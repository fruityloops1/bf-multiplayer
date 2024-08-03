#pragma once

#include "al/LiveActor/LiveActor.h"

namespace pe {

class PuppetSensorActor : public al::LiveActor {
public:
    PuppetSensorActor(const char* name)
        : LiveActor(name)
    {
    }

    void init(const al::ActorInitInfo& info) override;
    void attackSensor(al::HitSensor* me, al::HitSensor* other) override;
};

} // namespace pe
