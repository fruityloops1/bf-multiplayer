#pragma once

#include "al/LiveActor/LiveActor.h"

namespace al {

void calcJointPos(sead::Vector3f* out, const LiveActor* actor, const char* jointName);

} // namespace al
