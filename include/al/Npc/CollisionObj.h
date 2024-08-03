#pragma once

#include "al/HitSensor/HitSensor.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Resource/Resource.h"
#include <sead/math/seadMatrix.h>

namespace al {

class CollisionObj : public al::LiveActor {
public:
    CollisionObj(const ActorInitInfo& info, Resource*, const char* collisionPartsName, HitSensor* collisionSensor, const sead::Matrix34f*, const char*);
};

CollisionObj* createCollisionObj(LiveActor* resourceOwner, const ActorInitInfo& info, const char* collisionPartsName, HitSensor*, const char* collisionFollowJointName, const char* = nullptr);

} // namespace al