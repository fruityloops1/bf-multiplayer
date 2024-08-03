#pragma once

#include "al/LiveActor/LiveActor.h"

namespace al {

class ChildStep : public al::LiveActor {
    LiveActor* mHost = nullptr;
    sead::Vector3f _150;

public:
    ChildStep(const char* name, al::LiveActor* host);

    void init(const al::ActorInitInfo& info) override;
    bool receiveMsg(const SensorMsg* msg, HitSensor* other, HitSensor* me) override;
};

ChildStep* createChildStep(const al::ActorInitInfo& info, al::LiveActor* host, bool smth);
int calcChildStepCount(const al::ActorInitInfo& info);

} // namespace al
