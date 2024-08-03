#include "pe/Enet/Channels.h"
#include "pe/Enet/Types.h"

namespace pe {
namespace enet {

    ChannelType identifyType(const IPacket* packet)
    {
        for (int i = 0; i < sChannels.channelCount; i++) {
            if (*reinterpret_cast<void* const*>(packet) == sChannels.vtables[i])
                return (ChannelType)i;
        }
        PENET_ABORT("Packet could not be identified (vtable: %p)", *reinterpret_cast<void* const*>(packet));
    }

} // namespace enet
} // namespace pe