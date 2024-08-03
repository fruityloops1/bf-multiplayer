#pragma once

#include "pe/Enet/DataPacket.h"
#include "pe/Enet/IPacket.h"
#include "pe/Enet/RedistPacket.h"

namespace pe {
namespace enet {

    struct ActionPacketData {
        enum class Type : u8 {
            Anim,
            SubAnim,
            MaterialAnim,
            RaidonAnim
        };

        Type type;
        char action[127] { 0 };
    };

    using ToS_Action = RedistPacket<ActionPacketData>::ServerBound;
    using ToC_Action = RedistPacket<ActionPacketData>::ClientBound;

} // namespace enet
} // namespace pe
