#pragma once

#include "al/Draw/GraphicsStressDirector.h"
#include <basis/seadTypes.h>

namespace al {

class GraphicsSystemInfo {
public:
    u8 _0[0xD0];
    GraphicsStressDirector* mGraphicsStressDirector = nullptr;
};

} // namespace al
