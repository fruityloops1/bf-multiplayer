#pragma once

#include "al/LiveActor/ActorInitFunction.h"
#include "al/Placement/PlacementInfo.h"
#include "al/Resource/Resource.h"
#include <sead/math/seadVector.h>

namespace al {

    bool tryGetArg(float* out, const al::ActorInitInfo& info, const char* key);
    bool tryGetArg(int* out, const al::ActorInitInfo& info, const char* key);
    bool tryGetArg(bool* out, const al::ActorInitInfo& info, const char* key);
    bool tryGetStringArg(const char** out, const al::ActorInitInfo& info, const char* key);
    bool tryGetArg(float* out, const al::PlacementInfo& info, const char* key);
    bool tryGetArg(int* out, const al::PlacementInfo& info, const char* key);
    bool tryGetArg(bool* out, const al::PlacementInfo& info, const char* key);
    bool tryGetStringArg(const char** out, const al::PlacementInfo& info, const char* key);

    bool tryGetTrans(sead::Vector3f* out, const PlacementInfo& info);
    bool tryGetTrans(sead::Vector3f* out, const ActorInitInfo& info);

    bool tryGetInitFileIterAndName(PlacementInfo* out, sead::BufferedSafeString*, const al::Resource* res, const char*, const char*, const char*, const al::Resource* res2);

} // namespace al
