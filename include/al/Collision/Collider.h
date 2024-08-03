#pragma once

#include "al/Collision/CollisionDirector.h"
#include "math/seadMatrix.h"
#include "math/seadVector.h"
#include "types.h"

namespace al {

class LiveActor;
class Collider : IUseCollision {
    CollisionDirector* mCollisionDirector = nullptr;
    u64 unk[2];
    sead::Matrix34f* unk2;
    sead::Vector3f* unk3[2];

public:
    float mRadius;
    float mYOffset;
    u8 unk4[0x248];
};

float getColliderRadius(const LiveActor* actor);
float getColliderOffsetY(const LiveActor* actor);

} // namespace al