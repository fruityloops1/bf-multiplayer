#pragma once

#include "al/LiveActor/LiveActor.h"
#include <sead/math/seadQuat.h>

namespace pe {

class ClockMapPartsNet : public al::LiveActor {
    sead::Quatf mInitialQuat = sead::Quatf::unit;
    float mClockAngle = 0;
    int mRotateAxis = 0;
    int mRotateTime = 0;
    int mRotateSignTime = 0;
    int mDelayTime = 0;
    int mWaitTime = 0;
    int mRotateCount = 0;

    int getTotalCycleTime() const;
    const al::Nerve* getCurrentNerveByClock() const;
    int getProgressIntoNrvByClock() const;

public:
    ClockMapPartsNet(const char* name);

    void init(const al::ActorInitInfo& info) override;

    void exeWait();
    void exeRotate();
    void exeRotateEnd();
    void exeRotateSign();

    void control() override;
};

} // namespace pe
