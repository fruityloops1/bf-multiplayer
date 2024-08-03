#pragma once

#include <sead/math/seadMatrix.h>

namespace al {

class EffectSystemInfo;
class MtxPtrHolder;

class EffectKeeper {
public:
    EffectKeeper(const EffectSystemInfo*, const char* userName, const sead::Vector3f*, const sead::Vector3f*, const sead::Matrix34f*);
};

class IUseEffectKeeper {
    virtual EffectKeeper* getEffectKeeper() const = 0;
};

bool tryEmitEffect(IUseEffectKeeper* user, const char* effectName, const sead::Vector3f* at);
void tryDeleteEffect(IUseEffectKeeper* user, const char* effectName);

} // namespace al