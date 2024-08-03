#pragma once

#include <sead/math/seadMatrix.h>
#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>

namespace al {

class ActorPoseKeeperBase {
    sead::Vector3f mTrans { 0, 0, 0 };

public:
    ActorPoseKeeperBase() = default;

    virtual const sead::Vector3f& getRotate() const;
    virtual const sead::Vector3f& getScale() const;
    virtual const sead::Vector3f& getVelocity() const;
    virtual const sead::Vector3f& getFront() const;
    virtual const sead::Quatf& getQuat() const;
    virtual const sead::Vector3f& getGravity() const;
    virtual const sead::Matrix34f getMtx() const;

    virtual sead::Vector3f* getRotatePtr() const;
    virtual sead::Vector3f* getScalePtr() const;
    virtual sead::Vector3f* getVelocityPtr() const;
    virtual sead::Vector3f* getFrontPtr() const;
    virtual sead::Quatf* getQuatPtr() const;
    virtual sead::Vector3f* getGravityPtr() const;
    virtual sead::Matrix34f getMtxPtr() const;

    virtual void updatePoseRotate(const sead::Vector3f& rotate);
    virtual void updatePoseQuat(const sead::Quatf& quat);
    virtual void updatePoseMtx(const sead::Matrix34f& mtx);
    virtual void copyPose(const ActorPoseKeeperBase*);
    virtual void calcBaseMtx(sead::Matrix34f* mtx);
};

class ActorInitInfo;
class LiveActor;

void initActorPoseTFGSV(LiveActor* actor);
void initActorPoseTFSV(LiveActor* actor);
void initActorPoseTQSV(LiveActor* actor);
void initActorPoseTRMSV(LiveActor* actor);
void initActorPoseTRSV(LiveActor* actor);

void initActorSRT(LiveActor* actor, const ActorInitInfo& info);

const sead::Vector3f& getTrans(const LiveActor* actor);
const sead::Vector3f& getRotate(const LiveActor* actor);
const sead::Vector3f& getScale(const LiveActor* actor);
const sead::Vector3f& getFront(const LiveActor* actor);
const sead::Quatf& getQuat(const LiveActor* actor);
const sead::Vector3f& getGravity(const LiveActor* actor);

sead::Vector3f* getTransPtr(LiveActor* actor);
sead::Vector3f* getRotatePtr(LiveActor* actor);
sead::Vector3f* getScalePtr(LiveActor* actor);
sead::Vector3f* getFrontPtr(LiveActor* actor);
sead::Quatf* getQuatPtr(LiveActor* actor);
sead::Vector3f* getGravityPtr(LiveActor* actor);

void setTrans(LiveActor* actor, const sead::Vector3f& trans);
void setTrans(LiveActor* actor, float x, float y, float z);
void setTransX(LiveActor* actor, float x);
void setTransY(LiveActor* actor, float y);
void setTransZ(LiveActor* actor, float z);

void setRotate(LiveActor* actor, const sead::Vector3f& rotate);
void setRotateX(LiveActor* actor, float x);
void setRotateY(LiveActor* actor, float y);
void setRotateZ(LiveActor* actor, float z);

void setScale(LiveActor* actor, const sead::Vector3f& scale);
void setScale(LiveActor* actor, float x, float y, float z);
void setScale(LiveActor* actor, float scale);
// ?
void setScaleY(LiveActor* actor, float y);
void setScaleZ(LiveActor* actor, float z);

void setFront(LiveActor* actor, const sead::Vector3f& front);

void setQuat(LiveActor* actor, const sead::Quatf& quat);
void setGravity(LiveActor* actor, const sead::Vector3f& gravity);

void updatePoseQuat(LiveActor* actor, const sead::Quatf& quat);

void copyPose(LiveActor* to, const LiveActor* from);

void calcQuat(sead::Quatf* out, const LiveActor* actor);

} // namespace al