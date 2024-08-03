#pragma once

#include "pe/Enet/Channels.h"

namespace pe {
namespace enet {

    template <typename ClientType = void>
    class PacketHandler {
    public:
        using HandlePacketType = void (*)(IPacket*, ClientType*);
        struct PacketHandlerEntry {
            ChannelType type;
            HandlePacketType handler;
            bool useSeparateThread = false;
        };

        template <size_t N>
        PacketHandler(const PacketHandlerEntry (&entries)[N])
            : mEntries(entries)
            , mNumEntries(N)
        {
        }

        virtual void handlePacket(ChannelType type, const void* data, size_t len, ClientType* client = nullptr)
        {
            for (int i = 0; i < mNumEntries; i++) {
                const PacketHandlerEntry& entry = mEntries[i];
                if (entry.type == type) {
                    auto* func = sChannels.createFuncs[(int)type];
                    IPacket* packet = func(data, len);
                    mPacketsReceived[(int)type]++;
                    entry.handler(packet, client);
                    PENET_FREE(packet);
                    return;
                }
            }
            PENET_ABORT("Packet Handler for packet %hhu not found\n", type);
        }

        int getPacketsReceived(ChannelType type) const { return mPacketsReceived[(int)type]; }
        int getPacketsSent(ChannelType type) const { return mPacketsSent[(int)type]; }
        void increaseSentPacketCount(ChannelType type) { mPacketsSent[(int)type]++; }

    protected:
        const PacketHandlerEntry* mEntries = nullptr;
        size_t mNumEntries = 0;
        int mPacketsReceived[255] { 0 };
        int mPacketsSent[255] { 0 };
    };

} // namespace enet
} // namespace pe
