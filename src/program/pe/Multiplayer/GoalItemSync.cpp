#include "pe/Multiplayer/GoalItemSync.h"
#include "Game/MapObj/DisasterModeController.h"
#include "Game/MapObj/GoalItem.h"
#include "Game/MapObj/InkPatch.h"
#include "Game/MapObj/IslandHolder.h"
#include "Game/Scene/SingleModeScene.h"
#include "Game/System/IslandDataFunction.h"
#include "Game/System/SingleModeData.h"
#include "Game/System/SingleModeDataFunction.h"
#include "al/Base/String.h"
#include "al/LiveActor/LiveActorFlag.h"
#include "al/Nerve/NerveFunction.h"
#include "al/StageSwitch/StageSwitchKeeper.h"
#include "hook/trampoline.hpp"
#include "patch/code_patcher.hpp"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Log.h"
#include "pe/Util/Nerve.h"
#include "pe/Util/Type.h"
#include "prim/seadStringUtil.h"
#include <string>

namespace pe {

static bool sIsPhaseHookEnabled = true;

HOOK_DEFINE_TRAMPOLINE(GoalItemCollectNrv) { static void Callback(GoalItem * thisPtr); };
HOOK_DEFINE_TRAMPOLINE(SetPhaseEnd) { static void Callback(GameDataHolderWriter writer, bool isEnd); };

void GoalItemCollectNrv::Callback(GoalItem* thisPtr)
{
    Orig(thisPtr);
    pe::log("Shine collected");
    pe::log("Island ID: %d", thisPtr->mIslandID);
    pe::log("Scenario ID: %d", thisPtr->mID);

    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr)
        return;
    auto* client = mgr->getClient();
    if (client == nullptr)
        return;

    enet::ToS_GoalItemInfo packet;
    packet.islandID = thisPtr->mIslandID;
    packet.shineID = thisPtr->mID;
    client->sendPacket(&packet);
}

static enet::PhaseUnlockData makePhaseUnlockPacketData(const SingleModeData* sData, bool isEnd)
{
    enet::PhaseUnlockData data;
    data.unlockedPhase = sData->mUnlockedPhase;
    data.endCurrentPhase = isEnd;
    data.isNewToPhase1 = sData->mIsNewToPhase1;
    data.isNewToPhase2 = sData->mIsNewToPhase2;
    data.isNewToPhase3 = sData->mIsNewToPhase3;
    data.isNewToPhase4 = sData->mIsNewToPhase4;
    data.isNewToPhase2Boss = sData->mIsNewToPhase2Boss;
    data.isNewToPhase3Boss = sData->mIsNewToPhase3Boss;
    data.isNewToPhase1BowserIntro = sData->mIsNewToPhase1BowserIntro;
    data.isNewToPhase2BowserIntro = sData->mIsNewToPhase2BowserIntro;
    data.isNewToPhase3BowserIntro = sData->mIsNewToPhase3BowserIntro;
    data.isNewToPhase1BowserExit = sData->mIsNewToPhase1BowserExit;
    data.isNewToPhase2BowserExit = sData->mIsNewToPhase2BowserExit;
    data.isNewToPhase3BowserExit = sData->mIsNewToPhase3BowserExit;
    data.phase1DarkBowserHitPoint = sData->mPhase1DarkBowserHitPoint;
    data.phase2DarkBowserHitPoint = sData->mPhase2DarkBowserHitPoint;
    data.phase3DarkBowserHitPoint = sData->mPhase3DarkBowserHitPoint;
    data.phase4DarkBowserHitPoint = sData->mPhase4DarkBowserHitPoint;
    data.phase4DarkBowserHitPointFinal = sData->mPhase4DarkBowserHitPointFinal;
    data.phase3DarkBowserHitPointPreBattle = sData->mPhase3DarkBowserHitPointPreBattle;
    data.phase4DarkBowserHitPointPreBattle = sData->mPhase4DarkBowserHitPointPreBattle;
    return data;
}

void SetPhaseEnd::Callback(GameDataHolderWriter writer, bool isEnd)
{
    Orig(writer, isEnd);
    if (!sIsPhaseHookEnabled)
        return;

    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr)
        return;
    auto* client = mgr->getClient();
    if (client == nullptr)
        return;

    enet::ToS_PhaseUnlock packet(makePhaseUnlockPacketData(writer.mHolder->mData, isEnd));
    client->sendPacket(&packet);
}

void setScenarioIDHook(IslandHolder* thisPtr, int scenario, bool something)
{
    thisPtr->setScenarioID(scenario, something);
    if (!sIsPhaseHookEnabled)
        return;
    pe::log("Sending island %s scenario %d", thisPtr->getName(), scenario);

    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr)
        return;
    auto* client = mgr->getClient();
    if (client == nullptr)
        return;

    enet::ScenarioUpdateData data;
    std::strncpy(data.islandName, thisPtr->getName(), sizeof(data.islandName));
    data.scenario = scenario;
    data.unk = something;

    enet::ToS_ScenarioUpdate packet(data);
    client->sendPacket(&packet);
}

void initGoalItemSyncHooks()
{
    GoalItemCollectNrv::InstallAtOffset(0x00243e60);
    SetPhaseEnd::InstallAtOffset(0x00420060);

    using Patcher = exl::patch::CodePatcher;
    Patcher(0x0025cbfc).BranchLinkInst((void*)setScenarioIDHook);
    Patcher(0x0025cd5c).BranchLinkInst((void*)setScenarioIDHook);
    Patcher(0x0025d0e4).BranchLinkInst((void*)setScenarioIDHook);
}

void handleToC_GoalItemInfo(enet::ToC_GoalItemInfo* packet)
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr) {
        pe::warn("GoalItemInfo but no mgr");
    }

    // clang-format off
    ProductSequence* sequence = mgr->getSequence();
    if (sequence == nullptr or !util::isSequenceSingleModeNrv(sequence))
    {
        pe::warn("GoalItemInfo but wrong nerve");
        return;
    }
    // clang-format on

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(sequence->tryGetCurrentScene());
    if (scene == nullptr) {
        pe::warn("GoalItemInfo but no scene");
        return;
    }

    pe::log("Completed Island %d Shine %d", packet->islandID, packet->shineID);

    // Find the shine actor
    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (kit == nullptr)
        return;
    al::LiveActorGroup* allActors = kit->mAllActors;
    if (allActors == nullptr)
        return;

    GameDataHolderAccessor accessor(scene);

    if (packet->isCollectNewShine) {
        // search for shine actor
        GoalItem* shine = nullptr;
        for (int i = 0; i < allActors->mSize; i++) {
            al::LiveActor* actor = allActors->mActors[i];
            if (actor == nullptr)
                continue;
            if (GoalItem* curShine = util::typeCast<GoalItem>(actor)) {
                if (curShine->mIslandID == packet->islandID && curShine->mID == packet->shineID)
                    shine = curShine;
            }
        }

        auto* text = mgr->getStatusText();
        if (text) {
            const char* playerName = mgr->getPlayerName(packet->user);
            if (packet->islandID < 0) {
                text->log("%s collected Cat Shine!");
            } else {
                const char16* islandNameWide = IslandDataFunction::getIslandName(shine, text->getLayout(), packet->islandID);
                const char16* shineNameWide = IslandDataFunction::getIslandScenarioName(shine, text->getLayout(), packet->islandID, packet->shineID);
                char islandName[96] { 0 };
                char shineName[96] { 0 };
                size_t islandNameSize = std::char_traits<char16>::length(islandNameWide);
                size_t shineNameSize = std::char_traits<char16>::length(shineNameWide);
                sead::StringUtil::convertUtf16ToUtf8(islandName, sizeof(islandName), islandNameWide, islandNameSize);
                sead::StringUtil::convertUtf16ToUtf8(shineName, sizeof(shineName), shineNameWide, shineNameSize);

                text->log("%s collected %s: %s", playerName, islandName, shineName);
            }
        }

        accessor.mHolder->mData->mNumGoalItems = packet->numCollectedGoalItems - 1; // one is going to be added by completeScenario
        SingleModeDataFunction::completeScenario(accessor, { packet->islandID - 1, packet->shineID - 1 });
        SingleModeDataFunction::setGoalItemCheckpointPass(accessor, packet->islandID, packet->shineID);

        if (shine) {
            if (!al::isDead(shine))
                shine->kill();
            Lighthouse* hub = shine->mLighthouse;
            if (hub) {
                hub->setCurrentScenarioID(shine->mID);
                hub->killInkPillar();
                hub->setInkMeNot(true);
                hub->setScenarioAnim();
                hub->setScenarioButtons(shine->mLighthouse);
            }

            InkPatch* patch = shine->mInkPatch;
            if (patch)
                patch->disappear();
        } else {
            pe::warn("Couldn't find actor for GoalItem in island %d scenario %d", packet->islandID, packet->shineID);
        }
        mgr->queueShineCounterUpdate(true);

        DisasterModeController* controller = DisasterModeController::tryGetController(scene);
        if (controller->getNerveKeeper()->getCurrentNerve() == util::getNerveAt(0x01318b30) /* Disaster */)
            controller->endInstantly(true, true);
    } else {
        accessor.mHolder->mData->mNumGoalItems = packet->numCollectedGoalItems;
        mgr->queueShineCounterUpdate(false);
    }
}

void handleToC_PhaseUnlock(enet::ToC_PhaseUnlock* packet)
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr) {
        pe::warn("PhaseUnlock but no mgr");
    }

    // clang-format off
    ProductSequence* sequence = mgr->getSequence();
    if (sequence == nullptr or !util::isSequenceSingleModeNrv(sequence))
    {
        pe::warn("PhaseUnlock but wrong nerve");
        return;
    }
    // clang-format on

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(sequence->tryGetCurrentScene());
    if (scene == nullptr) {
        pe::warn("PhaseUnlock but no scene");
        return;
    }

    sIsPhaseHookEnabled = false;

    GameDataHolderAccessor accessor(scene);
    SingleModeData* sData = accessor.mHolder->mData;

    sData->mIsNewToPhase1 = packet->getData().isNewToPhase1;
    sData->mIsNewToPhase2 = packet->getData().isNewToPhase2;
    sData->mIsNewToPhase3 = packet->getData().isNewToPhase3;
    sData->mIsNewToPhase4 = packet->getData().isNewToPhase4;
    sData->mIsNewToPhase2Boss = packet->getData().isNewToPhase2Boss;
    sData->mIsNewToPhase3Boss = packet->getData().isNewToPhase3Boss;
    sData->mIsNewToPhase1BowserIntro = packet->getData().isNewToPhase1BowserIntro;
    sData->mIsNewToPhase2BowserIntro = packet->getData().isNewToPhase2BowserIntro;
    sData->mIsNewToPhase3BowserIntro = packet->getData().isNewToPhase3BowserIntro;
    sData->mIsNewToPhase1BowserExit = packet->getData().isNewToPhase1BowserExit;
    sData->mIsNewToPhase2BowserExit = packet->getData().isNewToPhase2BowserExit;
    sData->mIsNewToPhase3BowserExit = packet->getData().isNewToPhase3BowserExit;
    sData->mPhase1DarkBowserHitPoint = packet->getData().phase1DarkBowserHitPoint;
    sData->mPhase2DarkBowserHitPoint = packet->getData().phase2DarkBowserHitPoint;
    sData->mPhase3DarkBowserHitPoint = packet->getData().phase3DarkBowserHitPoint;
    sData->mPhase4DarkBowserHitPoint = packet->getData().phase4DarkBowserHitPoint;
    sData->mPhase4DarkBowserHitPointFinal = packet->getData().phase4DarkBowserHitPointFinal;
    sData->mPhase3DarkBowserHitPointPreBattle = packet->getData().phase3DarkBowserHitPointPreBattle;
    sData->mPhase4DarkBowserHitPointPreBattle = packet->getData().phase4DarkBowserHitPointPreBattle;
    SingleModeDataFunction::setUnlockedPhase(accessor, packet->getData().unlockedPhase);
    if (!sData->mIsPhaseEnd)
        SingleModeDataFunction::setPhaseEnd(accessor, packet->getData().endCurrentPhase);

    sIsPhaseHookEnabled = true;
}

void handleToC_ScenarioUpdate(enet::ToC_ScenarioUpdate* packet)
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr) {
        pe::warn("ScenarioUpdate but no mgr");
    }

    // clang-format off
    ProductSequence* sequence = mgr->getSequence();
    if (sequence == nullptr or !util::isSequenceSingleModeNrv(sequence))
    {
        pe::warn("ScenarioUpdate but wrong nerve");
        return;
    }
    // clang-format on

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(sequence->tryGetCurrentScene());
    if (scene == nullptr) {
        pe::warn("ScenarioUpdate but no scene");
        return;
    }

    sIsPhaseHookEnabled = false;

    GameDataHolderAccessor accessor(scene);
    for (IslandHolder& island : scene->mIslandKeeper->mIslands) {
        if (al::isEqualString(island.getName(), packet->getData().islandName)) {
            island.setScenarioID(packet->getData().scenario, packet->getData().unk);
            pe::log("got island %s scenario %d", island.getName(), packet->getData().scenario);
            break;
        }
    }

    sIsPhaseHookEnabled = true;
}

} // namespace pe
