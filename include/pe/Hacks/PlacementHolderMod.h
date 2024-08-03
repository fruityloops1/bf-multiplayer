#pragma once

#include <sead/heap/seadFrameHeap.h>

namespace pe {

void initPlacementHolderModHooks();
sead::FrameHeap*& getPlacementInfoHeap();
void createPlacementInfoHeap();

} // namespace pe
