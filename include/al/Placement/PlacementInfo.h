#pragma once

#include "al/Yaml/ByamlIter.h"
#include <sead/basis/seadTypes.h>

namespace al {

class PlacementInfo {
    ByamlIter mPlacementIter;
    ByamlIter mUnitConfigIter;
    PlacementInfo* _20 = nullptr;
    s32 _28;

public:
    PlacementInfo();

    void set(const ByamlIter& pPlacementIter, const ByamlIter& pUnitConfigIter, PlacementInfo*, s32);

    const ByamlIter& getPlacementIter() const { return mPlacementIter; }
    const ByamlIter& getUnitConfigIter() const { return mUnitConfigIter; }
};

} // namespace al