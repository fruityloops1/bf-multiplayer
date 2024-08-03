#pragma once

#include "al/Collision/MtxConnector.h"
#include "al/KeyPose/KeyPoseKeeper.h"
#include "al/LiveActor/LiveActor.h"
#include "math/seadVectorFwd.h"
#include <sead/math/seadQuat.h>

namespace pe {

class KeyMoveMapPartsNet : public al::LiveActor {
    al::KeyPoseKeeper* mKeyPoseKeeper = nullptr;
    sead::Vector3f mClippingVec;
    int mDelayTime = 0;
    bool mIsIgnoreFirstWaitTime = false;
    al::MtxConnector* mMtxConnector = nullptr;

    int calcKeyPoseFrames(int idx);
    int calcKeyPoseFramesTotal();
    int calcCurKeyPoseIdxBySyncClock(int* framesIntoPose = nullptr);
    void tryConnectToCollision();

public:
    KeyMoveMapPartsNet(const char* name);

    void init(const al::ActorInitInfo& info) override;
    void initAfterPlacement() override;
    void appear() override;
    void kill() override;

    void control() override;
};

} // namespace pe
