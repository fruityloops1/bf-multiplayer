#pragma once

#include "Game/Player/PlayerAnimFunc.h"
#include "al/Base/String.h"
#include "pe/DbgGui/Windows/Multiplayer.h"
#include "pe/Enet/Packets/Action.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/Packets/PuppetPose.h"
#include "pe/Enet/Packets/ToC_PlayerList.h"
#include "pe/Multiplayer/Puppets/PlayerPuppet.h"
#include "pe/Multiplayer/Puppets/RaidonPuppet.h"
#include "pe/Util/Account.h"
#include <nn/account.h>
#include <sead/container/seadPtrArray.h>

namespace pe {

class MultiplayerManager;
class PlayerData {
    enet::PlayerListDataEntry mListData;
    enet::PuppetPoseData mPoseData;
    sead::Vector3f mLastTrans;
    sead::Quatf mLastQuat;
    sead::Vector3f mLastRotate;
    float mLastAnimRate = 1.0f;
    float mLastBlendWeights[6] { 0 };
    EPlayerFigure mLastFigure = EPlayerFigure::None;
    EPlayerFigure mFigure = EPlayerFigure::None;
    puppets::PlayerPuppet* mPlayerPuppet = nullptr;
    puppets::RaidonPuppet* mRaidonPuppet = nullptr;
    const char* mLastRaidonAction = "";
    int mFrameCount = 0;
    s64 mLastPoseUpdateTimestamp = 0;
    s64 mPoseUpdateIntervalTicks = (nn::os::GetSystemTickFrequency() / 1000.f) * 200 /* good default value i think */;
    int mFramesWithZeroDelta = 0;

    bool mIsHostClient = false;
    bool mIsGhostClient = false;
    const al::Nerve* mLastDisasterModeControllerNrv = nullptr;

    struct StartActionQueueEntry {
        enet::ActionPacketData data;
        int framesLeft = sPosePacketInterval;
    };

    sead::PtrArray<StartActionQueueEntry> mActionQueue;

public:
    PlayerData(enet::PlayerListDataEntry packetData);

    ~PlayerData()
    {
        mActionQueue.freeBuffer();
    }

    void setPuppet(puppets::PlayerPuppet* puppet) { mPlayerPuppet = puppet; }
    void setRaidonPuppet(puppets::RaidonPuppet* puppet) { mRaidonPuppet = puppet; }
    puppets::PlayerPuppet* getPuppet() const { return mPlayerPuppet; }
    void setIsHostClient(bool isHostClient) { mIsHostClient = isHostClient; }

    const enet::PlayerListDataEntry& getListData() const { return mListData; }
    enet::PuppetPoseData& getPoseData() { return mPoseData; }

    void update(MultiplayerManager* mgr);
    void updateLocal(MultiplayerManager* mgr);

    void handlePosePacket(enet::ToC_PuppetPose& packet);
    void handleActionPacket(enet::ToC_Action& packet);
    void handleFigureChangePacket(enet::ToC_PlayerFigureChange& packet);

    bool isLocal() const
    {
        auto uid = util::getCurrentUserUid();
        return mListData.user.m_Storage[0] == uid.m_Storage[0] && mListData.user.m_Storage[1] == uid.m_Storage[1];
    }

    static bool isBlendedAnim(const sead::SafeString& anim)
    {
        constexpr const char* blendedAnimSubStrings[] { "Move" };
        for (const char* subString : blendedAnimSubStrings) {
            if (al::isEqualSubString(anim.cstr(), subString))
                return true;
        }
        return false;
    }

    static constexpr int sPosePacketInterval = 5;

    friend class gui::Multiplayer;
};

void resetRaidonSurfing();

} // namespace pe
