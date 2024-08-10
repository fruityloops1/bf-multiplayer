#include "pe/Multiplayer/MultiplayerManager.h"
#include "Game/MapObj/GoalItem.h"
#include "Game/Player/PlayerAnimator.h"
#include "Game/Scene/SingleModeScene.h"
#include "Game/Sequence/ProductStateSingleMode.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/SceneObjUtil.h"
#include "al/Controller/ControllerUtil.h"
#include "al/Layout/LayoutActor.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/LiveActorFlag.h"
#include "al/Player/PlayerHolder.h"
#include "heap/seadHeapMgr.h"
#include "hook/trampoline.hpp"
#include "patch/code_patcher.hpp"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Enet/NetClient.h"
#include "pe/Enet/Packets/Action.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/ProjectPacketHandler.h"
#include "pe/Hacks/RCSPlayers.h"
#include "pe/Multiplayer/GoalItemSync.h"
#include "pe/Multiplayer/MiscMapSync.h"
#include "pe/Multiplayer/MultiplayerStatusText.h"
#include "pe/Multiplayer/Puppets/PlayerPuppet.h"
#include "pe/Util/Account.h"
#include "pe/Util/Log.h"
#include "pe/Util/Nerve.h"
#include "pe/Util/Type.h"
#include "util/modules.hpp"

namespace pe {

/**
    Hooks
 */

static bool sStartAnimHookEnabled = true;

HOOK_DEFINE_TRAMPOLINE(StartAnimHook)
{
    static void Callback(PlayerAnimator * thisPtr, const sead::SafeString& anim);
};
HOOK_DEFINE_TRAMPOLINE(StartSubAnimHook)
{
    static void Callback(PlayerAnimator * thisPtr, const sead::SafeString& subAnim);
};
HOOK_DEFINE_TRAMPOLINE(StartMaterialAnimHook)
{
    static void Callback(PlayerAnimator * thisPtr, const sead::SafeString& matAnim);
};

bool nameplateHideModelFix(al::LiveActor* actor)
{
    if (PlayerActor* player = util::checkPlayer(actor))
        return rc::isPlayerHideModel(player);
    return al::isHideModel(actor);
}

HOOK_DEFINE_TRAMPOLINE(SingleModeSceneControlHook) { static void Callback(SingleModeScene * scene); };
void SingleModeSceneControlHook::Callback(SingleModeScene* scene)
{
    {
        sead::ScopedCurrentHeapSetter setter(pe::enet::getEnetHeap());
        pe::MultiplayerManager::instance()->execute(); // wanna replace this with a real executor, but its not working and im too lazy rn
    }

    // sick of this stupid mf
    // DisasterModeController::tryGetController(scene)->mFramesOfDisaster = 0;
    // DisasterModeController::tryGetController(scene)->mFramesOfDisasterTransition = 0;
    // DisasterModeController::tryGetController(scene)->mFramesOfAnticipation = 0;
    // DisasterModeController::tryGetController(scene)->mFramesOfProsperity = 999999999;
    // DisasterModeController::tryGetController(scene)->mDisableClock = true;
    Orig(scene);
}

HOOK_DEFINE_TRAMPOLINE(SingleModeSceneInitHook) { static void Callback(SingleModeScene*, const al::SceneInitInfo&); };
void SingleModeSceneInitHook::Callback(SingleModeScene* scene, const al::SceneInitInfo& info)
{
    Orig(scene, info);

    pe::enet::ToS_SingleModeSceneEnter packet;
    packet.playerType = (u8)pe::RCSPlayers::getCurrentPlayerType();
    pe::enet::getNetClient()->sendPacket(&packet);

    // al::registerExecutorUser(pe::MultiplayerManager::instance(), scene->mLiveActorKit->mExecuteDirector, "Multiplayer Manager");
    pe::MultiplayerManager::instance()->initAfterPlacement(scene);
    pe::resetRaidonSurfing();
    KoopaJr* koopaJr = rc::getKoopaJr(scene);
    if (koopaJr) {
        al::LiveActor* player = al::getPlayerActor(koopaJr, 0);
        if (player)
            al::setTrans(koopaJr, al::getTrans(player));
        al::setNerve(koopaJr, util::getNerveAt(0x0137f630));
    }
}

HOOK_DEFINE_TRAMPOLINE(SceneDestroy) { static void Callback(al::Scene * thisPtr); };

void SceneDestroy::Callback(al::Scene* thisPtr)
{
    MultiplayerManager::instance()->deinitPuppets();
    Orig(thisPtr);
}

void playerActorInitHook(PlayerActor* actor, const al::ActorInitInfo& info)
{
    al::initActorSRT(actor, info);
    pe::MultiplayerManager::instance()->initPuppets(actor, info);
}

void layoutInitHook(GuideGameWindow* thisPtr, const al::LayoutInitInfo& info, bool something)
{
    new (thisPtr) GuideGameWindow(info, something);

    pe::MultiplayerManager::instance()->initLayout(info);
}

/**
    MultiplayerManager
 */

SEAD_SINGLETON_DISPOSER_IMPL(MultiplayerManager)

void MultiplayerManager::execute()
{
    for (PlayerData& player : mPlayers)
        player.update(this);

    if (mLocalPlayer)
        mLocalPlayer->updateLocal(this);

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(mSequence->tryGetCurrentScene());

    if (mFramesUntilShineCounterUpdate != -1)
        mFramesUntilShineCounterUpdate--;
    if (mFramesUntilShineCounterUpdate == 0 && scene != nullptr) {
        scene->mLayout->getCounterGoalItemParts()->startDemo();
        scene->mLayout->getCounterGoalItemParts()->endDemo(!mIsShineCounterCountUp);
    }

    if (mStatusText)
        mStatusText->updateNerve();
}

void MultiplayerManager::init()
{
    enet::initialize(gui::getPeepaHeap());

    mNetClient = new enet::NetClient(new enet::ProjectPacketHandler);
    mNetClient->start();

    mPlayers.allocBuffer(sMaxPlayers, nullptr);
    for (int i = 0; i < 5; i++)
        mPuppetPools[i].allocBuffer(sMaxPlayers, nullptr);
    mRaidonPuppetPool.allocBuffer(sMaxPlayers, nullptr);

    mSequence = pe::gui::DbgGui::instance()->getSharedData().productSequence;

    RCSPlayers::readPlayerTypeFromFile();

    using Patcher = exl::patch::CodePatcher;

    StartAnimHook::InstallAtOffset(0x003747a0);
    StartSubAnimHook::InstallAtOffset(0x00374c60);
    StartMaterialAnimHook::InstallAtOffset(0x00375360);
    exl::patch::CodePatcher(0x00146958).BranchLinkInst((void*)nameplateHideModelFix);
    SingleModeSceneControlHook::InstallAtOffset(0x003e84d0);
    SingleModeSceneInitHook::InstallAtOffset(0x003e7090);
    SceneDestroy::InstallAtOffset(0x008a40e0);
    Patcher(0x003601ac).BranchLinkInst((void*)playerActorInitHook);
    initGoalItemSyncHooks();
    puppets::initPlayerBonkHook();
    installMiscMapSyncHooks();
    Patcher(0x003e7c28).BranchLinkInst((void*)layoutInitHook);
}

void MultiplayerManager::initAfterPlacement(SingleModeScene* scene)
{
    mRaidonSurf = scene->mRaidonSurf;
    for (auto& player : mPlayers) {
        auto* puppet = player.getPuppet();
        if (puppet == nullptr or !isInSingleModeScene())
            continue;
        puppet->mNameplate->init(player.getListData().name, false, false);
    }
    mStatusText = new MultiplayerStatusText(scene->mLayout);
}

void MultiplayerManager::destroyStatusText()
{
    delete mStatusText;
    mStatusText = nullptr;
}

void MultiplayerManager::initPuppets(PlayerActor* player, const al::ActorInitInfo& info)
{
    mPlayerActor = player;

    for (auto& pool : mPuppetPools)
        pool.clear();
    mRaidonPuppetPool.clear();

    for (int playerType = 0; playerType < 5; playerType++)
        for (int i = 0; i < sMaxPlayers; i++) {
            puppets::PlayerPuppet* puppet = new puppets::PlayerPuppet(util::PlayerType(playerType));
            al::initCreateActorNoPlacementInfo(puppet, info);
            puppet->change(EPlayerFigure::None);
            mPuppetPools[playerType].pushBack(puppet);
        }

    for (int i = 0; i < sMaxPlayers; i++) {
        puppets::RaidonPuppet* puppet = new puppets::RaidonPuppet;
        al::initCreateActorNoPlacementInfo(puppet, info);
        mRaidonPuppetPool.pushBack(puppet);
    }

    mPuppetsReady = true;
}

void MultiplayerManager::initLayout(const al::LayoutInitInfo& info)
{
    for (int playerType = 0; playerType < 5; playerType++)
        for (int i = 0; i < sMaxPlayers; i++) {
            puppets::PlayerPuppet* puppet = mPuppetPools[playerType][i];
            puppet->initLayout(info);
        }
}

void MultiplayerManager::onConnect()
{
    enet::PlayerFigureChangeData data;
    data.playerFigure = (u8)RCSPlayers::getCurrentPlayerType();
    enet::ToS_PlayerFigureChange packet(data);
    enet::getNetClient()->sendPacket(&packet);
}

void MultiplayerManager::handlePlayerListPacket(enet::ToC_PlayerList& packet)
{
    sead::ScopedCurrentHeapSetter setter(gui::getPeepaHeap());

    if (isInSingleModeScene()) {
        for (auto& pool : mPuppetPools)
            for (int i = 0; i < pool.size(); i++) {
                auto* puppet = pool[i];
                if (puppet)
                    puppet->kill();
            }
        for (int i = 0; i < mRaidonPuppetPool.size(); i++) {
            auto* puppet = mRaidonPuppetPool[i];
            if (puppet)
                puppet->kill();
        }
    }

    mPlayers.clear();
    if (mPlayers.capacity() < packet.size())
        mPlayers.allocBuffer(packet.size(), nullptr);

    if (packet.size() > sMaxPlayers)
        pe::err("PlayerList size is too big (%d > sMaxPlayers)", packet.size(), sMaxPlayers);

    {
        int puppetPoolIdx = 0;
        for (int i = 0; i < packet.size(); i++) {
            auto& entry = *packet[i];
            PlayerData* data = mPlayers.emplaceBack(entry);
            bool local = false;
            if (data->isLocal()) {
                local = true;
                data->setPuppet(nullptr);
            } else if (mPuppetsReady) {
                puppets::PlayerPuppet* puppet = mPuppetPools[(int)entry.playerType][puppetPoolIdx];
                puppets::RaidonPuppet* raidonPuppet = mRaidonPuppetPool[puppetPoolIdx];
                if (puppet) {
                    puppet->setPlayerData(data);
                    puppet->change((EPlayerFigure)entry.playerFigure);
                }
                if (raidonPuppet) {
                    raidonPuppet->setPlayerData(data);
                }
                data->setPuppet(puppet);
                data->setRaidonPuppet(raidonPuppet);
                puppetPoolIdx++;
            } else {
                pe::warn("Could not grab puppet for %s because puppets are not yet initialized", entry.name);
            }

            data->setIsHostClient(i == packet.size() - 1);

            if (local) {
                mLocalPlayer = data;
            }

            if (entry.isNew && mStatusText) {
                mStatusText->log("%s joined the game", entry.name);
            }
        }
    }
}

void MultiplayerManager::handlePuppetPosePacket(enet::ToC_PuppetPose& packet)
{
    PlayerData* player = getPlayer(packet.getSender());
    if (player) {
        if (player->isLocal()) {
            pe::err("PuppetPose received for local player");
            return;
        }
        player->handlePosePacket(packet);
    }
}

void MultiplayerManager::handleActionPacket(enet::ToC_Action& packet)
{
    PlayerData* player = getPlayer(packet.getSender());
    if (player) {
        if (player->isLocal()) {
            pe::err("Action received for local player");
            return;
        }
        player->handleActionPacket(packet);
    }
}

void MultiplayerManager::handleFigurePacket(enet::ToC_PlayerFigureChange& packet)
{
    PlayerData* player = getPlayer(packet.getSender());
    if (player) {
        if (player->isLocal()) {
            pe::err("FigureChange received for local player");
            return;
        }
        player->handleFigureChangePacket(packet);
    }
}

PlayerData* MultiplayerManager::getPlayer(nn::account::Uid uid)
{
    for (PlayerData& player : mPlayers) {
        auto user = player.getListData().user;
        if (user.m_Storage[0] == uid.m_Storage[0] && user.m_Storage[1] == uid.m_Storage[1])
            return &player;
    }
    return nullptr;
}

void MultiplayerManager::setStartAnimHookEnabled(bool enabled)
{
    sStartAnimHookEnabled = enabled;
}

static void sendAnimPacket(PlayerAnimator* thisPtr, const sead::SafeString& animName, enet::ActionPacketData::Type type)
{
    enet::ActionPacketData data;
    data.type = type;
    std::strncpy(data.action, animName.cstr(), sizeof(data.action));
    enet::NetClient* client = MultiplayerManager::instance()->getClient();

    enet::ToS_Action packet(data);
    client->sendPacket(&packet, false);
    client->flush();
}

void StartAnimHook::Callback(PlayerAnimator* thisPtr, const sead::SafeString& anim)
{
    if (sStartAnimHookEnabled)
        sendAnimPacket(thisPtr, anim, enet::ActionPacketData::Type::Anim);

    Orig(thisPtr, anim);
}

void StartSubAnimHook::Callback(PlayerAnimator* thisPtr, const sead::SafeString& subAnim)
{
    if (sStartAnimHookEnabled)
        sendAnimPacket(thisPtr, subAnim, enet::ActionPacketData::Type::SubAnim);

    Orig(thisPtr, subAnim);
}

void StartMaterialAnimHook::Callback(PlayerAnimator* thisPtr, const sead::SafeString& matAnim)
{
    if (sStartAnimHookEnabled)
        sendAnimPacket(thisPtr, matAnim, enet::ActionPacketData::Type::MaterialAnim);

    Orig(thisPtr, matAnim);
}

bool MultiplayerManager::isInSingleModeScene()
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr) {
        return false;
    }

    // clang-format off
    ProductSequence* sequence = mgr->getSequence();
    if (sequence == nullptr or !util::isSequenceSingleModeNrv(sequence))
    {
        return false;
    }
    // clang-format on

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(sequence->tryGetCurrentScene());
    if (scene == nullptr) {
        return false;
    }
    return true;
}

} // namespace pe
