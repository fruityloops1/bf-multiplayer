#pragma once

#include <sead/basis/seadTypes.h>
#include <sead/container/seadPtrArray.h>
#include <sead/math/seadMatrix.h>
#include <sead/math/seadVector.h>

namespace al {

class LiveActor;

enum class HitSensorType : u8 {
    Eye = 0,
    Player = 1,
    PlayerEye = 2,
    Npc = 3,
    Ride = 4,
    Enemy = 5,
    EnemyBody = 6,
    EnemyAttack = 7,
    KillerMagnum = 8,
    Dossun = 9,
    MapObj = 11,
    Bindable = 13,
    Collision = 14,
    KickKoura = 15,
    PlayerFireBall = 16,
    HoldObj = 18,
    BindableGigaBell = 19,
    BindableGoal = 20,
    BindableAllPlayer = 21,
    BindableBubbleOutScreen = 22,
    BindableKoura = 23,
    BindableRouteDokan = 24,
    BindableBubblePadInput = 25,
    MultiPlayer = 26,
    KoopaJr = 27,
    NpcAvoid = 29,
    BindableNpc = 30,
    BindableGoalItem = 31
};

class HitSensor {
    const char* const mName = nullptr;
    HitSensorType mType;
    u8 unk[0x34];
    al::LiveActor* mParentActor;
    const sead::Vector3f* mFollowPos;
    const sead::Matrix34f* mFollowMtx;

public:
    HitSensor(al::LiveActor* parent, const char* name, unsigned int, float, unsigned short, const sead::Vector3f*, const sead::Matrix34f*, const sead::Vector3f&);

    bool trySensorSort();
    void setFollowPosPtr(const sead::Vector3f*);
    void setFollowMtxPtr(const sead::Matrix34f*);
    void validate();
    void invalidate();
    void validateBySystem();
    void invalidateBySystem();
    void update();
    void addHitSensor(al::HitSensor* sensor);

    inline const char* const getName() { return mName; }
    HitSensorType getType() const { return mType; }
};

} // namespace al