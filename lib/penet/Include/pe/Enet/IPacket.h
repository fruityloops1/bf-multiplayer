#pragma once

#include "pe/Enet/Types.h"

namespace pe {
namespace enet {

    class IPacket {
    public:
        virtual ~IPacket() { }
        virtual size_t calcSize() const = 0;
        virtual void build(void* outData) const = 0;
        virtual void read(const void* data, size_t len) = 0;
    };

} // namespace enet
} // namespace pe
