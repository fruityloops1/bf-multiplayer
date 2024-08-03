#include "pe/Multiplayer/MiscMapSync.h"
#include "Game/Boss/DarkBowser.h"
#include "Game/Enemy/ShadowMario.h"
#include "Game/MapObj/JumpFlipPanel.h"
#include "Game/MapObj/Shards.h"
#include "Game/Npc/NekoNormal.h"
#include "Game/Npc/NekoParent.h"
#include "Game/Scene/SingleModeScene.h"
#include "Game/System/GameDataHolderAccessor.h"
#include "Game/System/SingleModeDataFunction.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/SceneObjUtil.h"
#include "al/Base/String.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorClippingFunction.h"
#include "al/LiveActor/LiveActor.h"
#include "al/LiveActor/LiveActorFlag.h"
#include "al/Nerve/NerveFunction.h"
#include "al/Scene/LiveActorKit.h"
#include "hook/trampoline.hpp"
#include "lib/patch/code_patcher.hpp"
#include "pe/Enet/NetClient.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Log.h"
#include "pe/Util/Math.h"
#include "pe/Util/Nerve.h"
#include "pe/Util/Type.h"
#include "util/modules.hpp"
#include "util/sys/rw_pages.hpp"

namespace pe {

static bool shadowMarioKill(al::LiveActor* actor)
{
    bool isFirstStep = al::isFirstStep(actor);
    if (isFirstStep) {
        enet::MiscMapSyncData data;
        data.type = enet::MiscMapSyncData::Type::KillActorByObjId;
        std::strncpy(data.killActorByObjId.objId, actor->getPlacementHolder()->mIdClone, sizeof(data.killActorByObjId.objId));

        enet::ToS_MiscMapSync packet(data);
        enet::getNetClient()->sendPacket(&packet);
    }
    return isFirstStep;
}

static bool shardGot(Shards* shard)
{
    bool isFirstStep = al::isFirstStep(shard);

    if (isFirstStep && !shard->mCollectedByHook) {
        enet::MiscMapSyncData data;
        data.type = enet::MiscMapSyncData::Type::ShardCollect;
        data.shardCollect.zoneID = shard->mZoneID;
        data.shardCollect.shardID = shard->mShardID;

        enet::ToS_MiscMapSync packet(data);
        enet::getNetClient()->sendPacket(&packet);
    }

    return isFirstStep;
}

bool shardGotFinalCheck(ShardsWatcher* thisPtr)
{
    Shards* shardPtr;

    __asm("mov %0, x19"
          : "=r"(shardPtr));

    if (shardPtr->mCollectedByHook)
        return false;
    return thisPtr->isFinalShard();
}

PATCH_DEFINE_ASM(ShardsSize1, "mov w0, #520");
PATCH_DEFINE_ASM(ShardsSize2, "mov w0, #520");
PATCH_DEFINE_ASM(ShardsSize3, "mov w0, #520");

HOOK_DEFINE_TRAMPOLINE(ShardsCtor) { static void Callback(Shards * thisPtr, const char* name, bool sth); };

void ShardsCtor::Callback(Shards* thisPtr, const char* name, bool sth)
{
    Orig(thisPtr, name, sth);
    thisPtr->mCollectedByHook = false;
}

static s64 sLastJumpFlipPanelPacket = 0;
static bool sJumpFlipPanelState = false;
static bool sFlipAllJumpPanel = false;

static bool jumpFlipPanelTrigCheck(JumpFlipPanel* thisPtr)
{
    if (sFlipAllJumpPanel)
        return true;
    bool isFlipMe = rc::isAnyPlayerJumpTrigOn(thisPtr);
    if (isFlipMe && !(thisPtr->isFlipping() && al::getNerveStep(thisPtr) < 25)) {
        const s64 last = sLastJumpFlipPanelPacket;
        sLastJumpFlipPanelPacket = nn::os::GetSystemTick().m_tick;
        if (pe::dif(sLastJumpFlipPanelPacket, last) > (nn::os::GetSystemTickFrequency() / 1000) * 32) {
            enet::JumpFlipData data { !thisPtr->mFlipState };
            enet::ToS_JumpFlip packet(data);
            enet::getNetClient()->sendPacket(&packet, false);
            sJumpFlipPanelState = !thisPtr->mFlipState;
            sFlipAllJumpPanel = true;
            return true;
        }
    }

    if (!thisPtr->isFlipping())
        return thisPtr->mFlipState != sJumpFlipPanelState;

    return false;
}

static void actorExecuteEnd(JumpFlipPanel* thisPtr)
{
    sFlipAllJumpPanel = false;
}

HOOK_DEFINE_TRAMPOLINE(BowserDamage)
{
    static void Callback(DarkBowser * thisPtr, int amount);
};

static bool sDisableBowserDamageHook = false;

static void sendDarkBowserHealthPacket(int newHealth)
{
    pe::log("Sending DarkBowserHealth packet with newHealth: %d", newHealth);
    enet::MiscMapSyncData data;
    data.type = enet::MiscMapSyncData::Type::DarkBowserHealth;
    data.darkBowserHealth.health = newHealth;

    enet::ToS_MiscMapSync packet(data);
    enet::getNetClient()->sendPacket(&packet);
}

void BowserDamage::Callback(DarkBowser* thisPtr, int amount)
{
    amount /= 2; // more players = need more health
    Orig(thisPtr, amount);
    if (sDisableBowserDamageHook)
        return;

    sendDarkBowserHealthPacket(thisPtr->mHealth);
}

static void setUnlockedCharHook(GameDataHolderWriter writer, int character)
{
    SingleModeDataFunction::setUnlockedChar(writer, character);

    enet::MiscMapSyncData data;
    data.type = enet::MiscMapSyncData::Type::ToadUnlock;
    data.toadUnlock.character = character;
    enet::ToS_MiscMapSync packet(data);
    enet::getNetClient()->sendPacket(&packet);
}

static bool isActionEndShard(Shards* shard)
{
    if (shard->mCollectedByHook)
        return true;
    return al::isActionEnd(shard);
}

HOOK_DEFINE_TRAMPOLINE(KameckKill) { static void Callback(al::LiveActor * thisPtr); };

void KameckKill::Callback(al::LiveActor* thisPtr)
{
    Orig(thisPtr);

    enet::MiscMapSyncData data;
    data.type = enet::MiscMapSyncData::Type::KillActorByObjId;
    std::strncpy(data.killActorByObjId.objId, thisPtr->getPlacementHolder()->mIdClone, sizeof(data.killActorByObjId.objId));
    enet::ToS_MiscMapSync packet(data);
    enet::getNetClient()->sendPacket(&packet);
}

static void donketsuKill(al::LiveActor* thisPtr)
{
    thisPtr->::al::LiveActor::kill();

    enet::MiscMapSyncData data;
    data.type = enet::MiscMapSyncData::Type::KillActorByObjId;
    std::strncpy(data.killActorByObjId.objId, thisPtr->getPlacementHolder()->mIdClone, sizeof(data.killActorByObjId.objId));
    enet::ToS_MiscMapSync packet(data);
    enet::getNetClient()->sendPacket(&packet);
}

static void* donketsuVtable[58] { 0 };

HOOK_DEFINE_TRAMPOLINE(DonketsuVtableInit) { static void Callback(al::LiveActor * thisPtr, const char* name); };
void DonketsuVtableInit::Callback(al::LiveActor* thisPtr, const char* name)
{
    Orig(thisPtr, name);
    *(void**)thisPtr = &donketsuVtable[2]; // so so so sorry
}

void installMiscMapSyncHooks()
{
    using Patcher = exl::patch::CodePatcher;

    // misc object killers
    Patcher(0x0013cf30).BranchLinkInst((void*)shadowMarioKill);
    KameckKill::InstallAtOffset(0x000e4120);
    // codepatcher is evil im sorry
    memcpy(donketsuVtable, (const void*)exl::util::modules::GetTargetOffset(0x012a6558), sizeof(donketsuVtable));
    donketsuVtable[11] = (void*)donketsuKill;
    DonketsuVtableInit::InstallAtOffset(0x000ccdf0);

    // Shards
    ShardsSize1::InstallAtOffset(0x004292f0);
    ShardsSize2::InstallAtOffset(0x003de0d0);
    ShardsSize3::InstallAtOffset(0x002b9630);
    ShardsCtor::InstallAtOffset(0x002b39a0);
    Patcher(0x002b56b8).BranchLinkInst((void*)shardGot);
    Patcher(0x002b5710).BranchLinkInst((void*)shardGotFinalCheck);
    Patcher(0x002b57b8).BranchLinkInst((void*)isActionEndShard);

    // jump flip
    Patcher(0x00264e84).BranchLinkInst((void*)jumpFlipPanelTrigCheck);
    Patcher(0x00264b5c).BranchLinkInst((void*)jumpFlipPanelTrigCheck);
    Patcher(0x0080b2ac).BranchInst((void*)actorExecuteEnd); // EXTREMELY lazy

    // bowser health sync
    BowserDamage::InstallAtOffset(0x00012ae0);

    // toads
    Patcher(0x00311ec4).BranchLinkInst((void*)setUnlockedCharHook);
}

void handleToC_MiscMapSync(enet::ToC_MiscMapSync* packet)
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr) {
        pe::warn("MiscMapSync but no mgr");
    }

    // clang-format off
    ProductSequence* sequence = mgr->getSequence();
    if (sequence == nullptr or !util::isSequenceSingleModeNrv(sequence))
    {
        pe::warn("MiscMapSync but wrong nerve");
        return;
    }
    // clang-format on

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(sequence->tryGetCurrentScene());
    if (scene == nullptr) {
        pe::warn("MiscMapSync but no scene");
        return;
    }
    GameDataHolderAccessor accessor(scene);

    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (kit == nullptr)
        return;

    al::LiveActorGroup* allActors = kit->mAllActors;
    if (allActors == nullptr)
        return;

    /**
     *   ShardCollect
     */
    if (packet->getData().type == enet::MiscMapSyncData::Type::ShardCollect) {
        int zoneID = packet->getData().shardCollect.zoneID;
        int shardID = packet->getData().shardCollect.shardID;
        if (SingleModeDataFunction::isShardCollected(accessor, zoneID - 1, shardID - 1))
            return;
        SingleModeDataFunction::collectShard(accessor, zoneID - 1, shardID - 1);

        // Textbox crash (???)
        /*auto* sceneLayout = rc::getSingleModeSceneLayout(scene);
        if (sceneLayout && sceneLayout->mShardCounterParts->isAlive())
            sceneLayout->addShard(zoneID - 1, shardID, true, false);*/

        bool allCollect = true;
        for (int i = 1; i < 6; i++) {
            if (!SingleModeDataFunction::isShardCollected(accessor, zoneID - 1, i - 1))
                allCollect = false;
        }

        for (int i = 0; i < allActors->mSize; i++) {
            if (allActors->mActors[i] == nullptr)
                continue;

            al::LiveActor* actor = allActors->mActors[i];

            if (Shards* shard = util::typeCast<Shards>(actor)) {
                if (shard->mZoneID == zoneID) {
                    if (allCollect) { // let the other party grab the shine
                        shard->kill();
                        shard->mEmptyShardModel->kill();
                    } else if (shard->mShardID == shardID) { // if it's the shard they collected, collect
                        shard->mCollectedByHook = true;
                        if (al::isDead(shard))
                            shard->appear();
                        shard->endClipped();
                        shard->endClippedByLod();
                        shard->endFarLod();
                        al::invalidateClipping(shard);
                        al::setNerve(shard, util::getNerveAt(0x01350a50 /* Got */));
                    }
                }
            }
        }
    }

    /**
     *   KillActorByObjId
     */
    else if (packet->getData().type == enet::MiscMapSyncData::Type::KillActorByObjId)
        for (int i = 0; i < allActors->mSize; i++) {
            if (allActors->mActors[i] == nullptr)
                continue;

            al::LiveActor* actor = allActors->mActors[i];

            if (!actor->getPlacementHolder()->mKilledByHook && actor->getPlacementHolder()->mIdClone && al::isEqualString(actor->getPlacementHolder()->mIdClone, packet->getData().killActorByObjId.objId)) {
                actor->kill();
                actor->getPlacementHolder()->mKilledByHook = true;
            }
        }

    /**
     *   DarkBowserHealth
     */
    else if (packet->getData().type == enet::MiscMapSyncData::Type::DarkBowserHealth) {
        for (int i = 0; i < allActors->mSize; i++) {
            if (allActors->mActors[i] == nullptr)
                continue;

            al::LiveActor* actor = allActors->mActors[i];

            if (DarkBowser* bowser = util::typeCast<DarkBowser>(actor)) {
                int newHealth = packet->getData().darkBowserHealth.health;
                pe::log("got DarkBowserHealth packet newHealth: %d bowser->mHealth: %d", newHealth, bowser->mHealth);

                // um
                if (false && newHealth > bowser->mHealth) {
                    static s64 lastPacket = nn::os::GetSystemTick();
                    if (nn::os::GetSystemTick().m_tick - lastPacket > nn::os::GetSystemTickFrequency()) {
                        // wtf !! go back down
                        sendDarkBowserHealthPacket(bowser->mHealth);
                        lastPacket = nn::os::GetSystemTick().m_tick;
                    }
                } else if (bowser->mHealth != newHealth) {
                    int toDamage = bowser->mHealth - newHealth;
                    sDisableBowserDamageHook = true;
                    bowser->requestDamage(toDamage);
                    sDisableBowserDamageHook = false;
                }
                break;
            }
        }
    }

    /**
     *   ToadUnlock
     */
    else if (packet->getData().type == enet::MiscMapSyncData::Type::ToadUnlock) {
        pe::log("got ToadUnlock %d", packet->getData().toadUnlock.character);
        SingleModeDataFunction::setUnlockedChar(accessor, packet->getData().toadUnlock.character);
    }
}

void handleToC_JumpFlip(enet::ToC_JumpFlip* packet)
{
    sJumpFlipPanelState = packet->getData().flipState;
}

} // namespace pe
