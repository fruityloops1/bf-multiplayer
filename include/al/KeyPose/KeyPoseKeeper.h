#pragma once

#include "al/KeyPose/KeyPose.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "math/seadVectorFwd.h"

namespace al {

class KeyPoseKeeper {
    enum MoveType {
        MoveType_Loop,
        MoveType_Turn,
        MoveType_Stop,
        MoveType_Restart
    };

    KeyPose* mKeyPoses;
    int mKeyPoseAmount;
    int mCurrentKeyPoseIdx;
    union {
        int mMoveTypeInt;
        MoveType mMoveType;
    };
    bool _10;
    bool _11;

public:
    KeyPoseKeeper();

    void init(const ActorInitInfo& info);

    bool isFirstKey() const;
    bool isLastKey() const;
    const KeyPose* getCurrentKeyPose() const;
    const KeyPose* getNextKeyPose() const;
    const KeyPose* getKeyPose(int idx) const { return &mKeyPoses[idx]; };

    void next();
    void reset();
    void reverse();

    int getCurrentIdx() const { return mCurrentKeyPoseIdx; }
    void setCurrentIdx(int idx) { mCurrentKeyPoseIdx = idx; }
};

const sead::Vector3f& getCurrentKeyTrans(const KeyPoseKeeper* p);
const sead::Vector3f& getNextKeyTrans(const KeyPoseKeeper* p);
const PlacementInfo* getNextKeyPlacementInfo(const KeyPoseKeeper* p);
int getKeyPoseCount(const KeyPoseKeeper* p);
int calcKeyMoveMoveTime(const KeyPoseKeeper* p);
KeyPoseKeeper* createKeyPoseKeeper(const al::ActorInitInfo& info);
void setKeyMoveClippingInfo(const LiveActor* actor, sead::Vector3f*, const KeyPoseKeeper* keyPoseKeeper);
void calcLerpKeyTrans(sead::Vector3f* out, const KeyPoseKeeper* p, float progress);
void calcSlerpKeyQuat(sead::Quatf* out, const KeyPoseKeeper* p, float progress);

} // namespace al
