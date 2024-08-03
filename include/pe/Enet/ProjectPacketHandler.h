#pragma once

#include "pe/Enet/PacketHandler.h"

namespace pe {
namespace enet {

    class ProjectPacketHandler : public PacketHandler<void> {
    public:
        ProjectPacketHandler();
    };

} // namespace enet
} // namespace pe