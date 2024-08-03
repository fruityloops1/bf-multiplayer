#pragma once

#include "Game/Player/PlayerActor.h"
#include "al/LiveActor/LiveActor.h"

namespace pe {

class Foomin : public al::LiveActor {
    constexpr static int sFlipStartFrame = 60, sFlipEndFrame = 90;
    float pFlingVelocityMultiplier = 20;
    float pFlingYVelocityMultiplier = 20;
    bool pMoveLeftRight = false;
    float pMoveAmount = 500;
    float pMoveSpeed = 10;

    bool mMoveDir = false;
    int mMoveAmount = 0;
    int mFramesUntilCollisionPartsReactivate = 20;
    int mFramesSinceLaunch = -1;

    PlayerActor* mPlayerActor = nullptr;

    void invalidateCollisionParts();

public:
    Foomin(const char* name)
        : LiveActor(name)
    {
    }

    void init(const al::ActorInitInfo& info) override;
    bool receiveMsg(const al::SensorMsg* msg, al::HitSensor* source, al::HitSensor* target) override;
    void control() override;

    void exeWait();
    void exeFlip();

    void movement() override { LiveActor::movement(); }
};

} // namespace pe