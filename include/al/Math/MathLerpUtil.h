#pragma once

#include <sead/math/seadVectorFwd.h>

namespace al {

float lerpValue(float min, float max, float amount);
void lerpVec(sead::Vector3f* out, const sead::Vector3f& from, const sead::Vector3f& to, float amount);

} // namespace al
