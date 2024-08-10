#pragma once

#include "Game/MapObj/RaidonSurf.h"
#include "Game/Player/PlayerActor.h"
#include "al/Execute/ExecuteDirector.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "container/seadObjArray.h"
#include "container/seadPtrArray.h"
#include "pe/DbgGui/Windows/Multiplayer.h"
#include "pe/Enet/NetClient.h"
#include "pe/Enet/Packets/Action.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/Packets/PuppetPose.h"
#include "pe/Enet/Packets/ToC_PlayerList.h"
#include "pe/Multiplayer/MultiplayerStatusText.h"
#include "pe/Multiplayer/PlayerData.h"
#include "pe/Multiplayer/Puppets/PlayerPuppet.h"
#include "pe/Multiplayer/Puppets/RaidonPuppet.h"
#include "prim/seadSafeString.h"
#include <sead/heap/seadDisposer.h>

class SingleModeScene;
class GoalItem;

namespace pe {

class MultiplayerManager : public al::IUseExecutor {
    enet::NetClient* mNetClient = nullptr;

    sead::ObjArray<PlayerData> mPlayers;
    PlayerData* mLocalPlayer = nullptr;

    bool mPuppetsReady = false;
    sead::PtrArray<puppets::PlayerPuppet> mPuppetPools[5];
    sead::PtrArray<puppets::RaidonPuppet> mRaidonPuppetPool;

    ProductSequence* mSequence = nullptr;
    PlayerActor* mPlayerActor = nullptr;
    RaidonSurf* mRaidonSurf = nullptr;

    int mFramesUntilShineCounterUpdate = -1;
    bool mIsShineCounterCountUp = false;
    GoalItem* mShineByIslandScenario[20][5] { { nullptr } };

    MultiplayerStatusText* mStatusText = nullptr;

    PlayerData* getPlayer(nn::account::Uid uid);

    SEAD_SINGLETON_DISPOSER(MultiplayerManager)

public:
    MultiplayerManager() { }

    void execute() override;

    void init();
    void initAfterPlacement(SingleModeScene* scene);
    void initPuppets(PlayerActor* player, const al::ActorInitInfo& info);
    void deinitPuppets() { mPuppetsReady = false; }
    void initLayout(const al::LayoutInitInfo& info);
    void onConnect();

    void handlePlayerListPacket(enet::ToC_PlayerList& packet);
    void handlePuppetPosePacket(enet::ToC_PuppetPose& packet);
    void handleActionPacket(enet::ToC_Action& packet);
    void handleFigurePacket(enet::ToC_PlayerFigureChange& packet);

    ProductSequence* getSequence() const { return mSequence; }
    static void setStartAnimHookEnabled(bool enabled);
    enet::NetClient* getClient() const { return mNetClient; }

    void queueShineCounterUpdate(bool countUp)
    {
        mIsShineCounterCountUp = countUp;
        mFramesUntilShineCounterUpdate = 20;
    }

    PlayerActor* getPlayerActor() const { return mPlayerActor; }
    MultiplayerStatusText* getStatusText() const { return mStatusText; }
    void destroyStatusText();

    const char* getPlayerName(nn::account::Uid uid)
    {
        for (auto& entry : mPlayers) {
            auto& data = entry.getListData();
            if (data.user.m_Storage[0] == uid.m_Storage[0] && data.user.m_Storage[1] == uid.m_Storage[1])
                return data.name;
        }
        return "NULL";
    }

    constexpr static int sMaxPlayers = 5;
    constexpr static int sPosePacketInterval = 6;

    friend class PlayerData;
    friend class gui::Multiplayer;

    static bool isInSingleModeScene();
};

} // namespace pe
