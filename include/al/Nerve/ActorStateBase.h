#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/Nerve/NerveStateBase.h"

namespace al {

class ActorStateBase : public NerveStateBase {
protected:
    LiveActor* const mParent = nullptr;

public:
    ActorStateBase(const char* name, LiveActor* parent)
        : NerveStateBase(name)
        , mParent(parent)
    {
    }
};

} // namespace al