#pragma once

#include "al/Placement/PlacementInfo.h"
#include "al/Yaml/ByamlIter.h"
#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>

namespace al {

class KeyPose {
    sead::Quatf mQuat;
    sead::Vector3f mTrans;
    const ByamlIter* mPlacementInfo;

public:
    KeyPose();

    void init(const PlacementInfo& info);

    const sead::Quatf& getQuat() const { return mQuat; }
    const sead::Vector3f& getTrans() const { return mTrans; }
    const ByamlIter* getPlacementInfo() const { return mPlacementInfo; }
};

}