#pragma once

#include "al/Nerve/NerveExecutor.h"

namespace al {

class NerveStateBase : public NerveExecutor {
    bool mIsDead = true;

public:
    NerveStateBase(const char* name);

    virtual void init();
    virtual void appear();
    virtual void kill();
    virtual bool update();
    virtual void control();
};

} // namespace al