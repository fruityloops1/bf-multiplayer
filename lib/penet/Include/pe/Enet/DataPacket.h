#pragma once

#include "pe/Enet/IPacket.h"
#include "pe/Enet/Impls.h"
#include <cstring>
#include <type_traits>

namespace pe {
namespace enet {

    template <typename T>
    class DataPacket : public IPacket {
        void* getData() const
        {
            return reinterpret_cast<void*>(uintptr_t(this) + sizeof(DataPacket<T>));
        }

        size_t getSize() const
        {
            return sizeof(T) - sizeof(DataPacket<T>);
        }

    public:
        size_t calcSize() const override
        {
            return getSize();
        }

        void build(void* outData) const override
        {
            if (getSize())
                std::memcpy(outData, getData(), getSize());
        }

        void read(const void* data, size_t len) override
        {
            if (len != getSize())
                PENET_ABORT("pe::enet::DataPacket::read: Wrong packet size (%zu != %zu)", len, getSize());
            if (getSize())
                std::memcpy(getData(), data, getSize());
        }
    };

} // namespace enet
} // namespace pe
