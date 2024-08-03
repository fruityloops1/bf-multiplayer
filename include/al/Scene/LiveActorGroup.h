#pragma once

#include "al/LiveActor/LiveActor.h"
#include <sead/container/seadPtrArray.h>

namespace al {

class LiveActorGroup {
public:
    const char* const mName = nullptr;
    s32 mCapacity = 0;
    s32 mSize = 0;
    LiveActor** mActors;

    LiveActorGroup(const char* name, int max);
    virtual void registerActor(LiveActor*);
};

} // namespace al