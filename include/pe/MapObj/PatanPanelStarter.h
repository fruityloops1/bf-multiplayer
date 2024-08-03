#pragma once

#include "al/LiveActor/LiveActor.h"

namespace pe {

class PatanPanelStarter : public al::LiveActor {
public:
    PatanPanelStarter(const char* name)
        : al::LiveActor(name)
    {
    }

    void init(const al::ActorInitInfo& info) override;
    bool receiveMsg(const al::SensorMsg* msg, al::HitSensor* source, al::HitSensor* target) override;

    void exeOffWait();
    void exeOnWait();
    void exeToOff();
    void exeToOn();

    void close();
};

} // namespace pe