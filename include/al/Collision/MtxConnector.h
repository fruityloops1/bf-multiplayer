#pragma once

#include "al/LiveActor/LiveActor.h"
#include <sead/basis/seadTypes.h>
#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>

namespace al {

class MtxConnector {
    u8 unk[0x5c];

public:
    MtxConnector(const sead::Quatf& quat, const sead::Vector3f& trans);
};

MtxConnector* tryCreateMtxConnector(LiveActor* actor, const ActorInitInfo& info);

void attachMtxConnectorToCollision(MtxConnector* connector, const LiveActor* actor, bool);
void connectPoseQT(LiveActor* actor, const MtxConnector* connector);

} // namespace al