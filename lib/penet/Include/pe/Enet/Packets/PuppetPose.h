#pragma once

#include "pe/Enet/DataPacket.h"
#include "pe/Enet/IPacket.h"
#include "pe/Enet/RedistPacket.h"

namespace pe {
namespace enet {

    struct PuppetPoseData {
        sead::Vector3f trans { 0, 0, 0 };
        sead::Quatf quat = sead::Quatf::unit;
        float animRate = -1;
        float blendWeights[6];
        bool isSurfingRaidon = false;
    };

    using ToS_PuppetPose = RedistPacket<PuppetPoseData>::ServerBound;
    using ToC_PuppetPose = RedistPacket<PuppetPoseData>::ClientBound;

} // namespace enet
} // namespace pe
