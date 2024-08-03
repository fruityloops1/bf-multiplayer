#pragma once

#include "al/HitSensor/HitSensor.h"
#include <sead/container/seadPtrArray.h>

namespace al {

class HitSensorKeeper {
    sead::PtrArray<HitSensor> mHitSensors;

public:
};

} // namespace al