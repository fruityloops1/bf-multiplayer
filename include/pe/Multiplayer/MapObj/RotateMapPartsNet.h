#pragma once

#include "al/LiveActor/LiveActor.h"

namespace pe {

class RotateMapPartsNet : public al::LiveActor {
    sead::Quatf mInitialQuat = sead::Quatf::unit;
    float mClockAngle = 0;
    int mRotateAxis = 0;
    float mRotateSpeed = 0;
    bool mIsTriggerEffectOnAngle = false;
    float mEffectTriggerAngle = 0;

    int getTotalCycleTime() const;
    const al::Nerve* getCurrentNerveByClock() const;
    int getProgressIntoNrvByClock() const;

public:
    RotateMapPartsNet(const char* name);

    void init(const al::ActorInitInfo& info) override;

    void start();

    void exeStandBy();
    void exeRotate();
};

} // namespace pe
