#pragma once

#include "al/Screen/ScreenPointTarget.h"
#include <sead/container/seadPtrArray.h>

namespace al {

class ScreenPointKeeper {
    sead::PtrArray<ScreenPointTarget> mScreenPointTargets;
};

} // namespace al