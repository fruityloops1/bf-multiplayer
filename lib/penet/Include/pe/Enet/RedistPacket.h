#pragma once

#include "pe/Enet/DataPacket.h"
#include "pe/Enet/Impls.h"
#include <cstring>
#include <type_traits>

namespace pe {
namespace enet {

    template <typename T>
    struct RedistPacket {
        struct ServerBound : public DataPacket<ServerBound> {
            T mPacketData;

        public:
            ServerBound() { }
            ServerBound(const T& packetData)
                : mPacketData(packetData)
            {
            }

            const T& getData() const { return mPacketData; }
        };

        class ClientBound : public DataPacket<ClientBound> {
            nn::account::Uid mUid;
            T mPacketData;

        public:
            ClientBound() { }
            ClientBound(const nn::account::Uid& uid, const T& packetData)
                : mUid(uid)
                , mPacketData(packetData)
            {
            }

            const T& getData() { return mPacketData; }
            const nn::account::Uid getSender() const { return mUid; }
        };
    };

} // namespace enet
} // namespace pe
