#pragma once

#include "enet/enet.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/Types.h"
#include "pe/ServerUtil.h"
#include <functional>

namespace pe {
namespace enet {

    class Server;
    inline size_t calcEnetAddressHashCode(ENetAddress address)
    {
        size_t hash = std::hash<u32>()(address.host);
        hash ^= std::hash<u16>()(address.port);
        return hash;
    }

    class Client {
        Server* mServer = nullptr;
        ENetPeer* mPeer = nullptr;
        bool mHasGreeting = false;
        nn::account::Uid mUid { 0 };
        nn::account::Nickname mName { 0 };

        bool mIsGhost = false;
        sead::Vector3f mTrans { 0, 0, 0 };
        u8 mPlayerType = 0;
        u8 mPlayerFigure = 0;

    public:
        Client() { }
        Client(Server* parent, ENetPeer* peer);

        void sendPacket(IPacket* packet, bool reliable = true);

        bool hasGreeted() const { return mHasGreeting; }
        size_t getHash() const { return calcEnetAddressHashCode(mPeer->address); }
        ENetPeer* getPeer() const { return mPeer; }

        void handleGreet(ToS_Hello* packet);
        void uploadSaveThread();
        void disconnect();

        friend struct Handlers;
        friend class Console;
        friend class Server;
        friend class pe::ServerUtil;
    };

} // namespace enet
} // namespace pe