#include "pe/Enet/Client.h"
#include "enet/enet.h"
#include "pe/Enet/Channels.h"
#include "pe/Enet/Console.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/Server.h"
#include "pe/ServerUtil.h"
#include <cstdio>
#include <thread>
#include "pe/Util.h"

namespace pe {
namespace enet {

    Client::Client(Server* parent, ENetPeer* peer)
        : mServer(parent)
        , mPeer(peer)
    {
        ENetAddress address = peer->address;

        Console::log("New client connected from %s:%u (%zx)", pe::InetNtoa(address.host), address.port, getHash());
    }

    void Client::sendPacket(IPacket* packet, bool reliable)
    {
        if (mPeer->state != ENET_PEER_STATE_CONNECTED)
            return;
        size_t len = packet->calcSize();
        u8* buf = new u8[len];
        packet->build(buf);

        ENetPacket* pak = enet_packet_create(buf, len, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        enet_peer_send(mPeer, (int)identifyType(packet), pak);
        delete[] buf;
    }

    void Client::handleGreet(ToS_Hello* packet)
    {
        mIsGhost = packet->isGhost;
        mHasGreeting = true;
        mUid = packet->uid;
        std::memcpy(&mName, packet->name, sizeof(nn::account::Nickname));

        ToC_PlayerList listPacket = ServerUtil::makePlayerListPacket(mServer, this);
        mServer->sendPacketToAll(&listPacket);

        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds time(std::chrono::duration_cast<std::chrono::microseconds>(now - mServer->getSyncClockStartTimestamp()));
        ToC_SyncClockUpdate clockPacket;
        clockPacket.microseconds = time.count();
        mServer->sendPacketToAll(&clockPacket, true);
        mServer->flush();

        std::thread thread(&Client::uploadSaveThread, this);
        thread.detach();
    }

    void Client::uploadSaveThread()
    {
        mServer->getSaveDataMutex().lock();

        for (int i = 0; i < 8; i++) {
            SaveDataChunk packet;
            packet.chunkIndex = i;
            packet.size = i == 7 ? (mServer->getSaveData().sharedSaveSize - enet::sSaveDataChunkSize * 7) : enet::sSaveDataChunkSize;
            memcpy(packet.data, mServer->getSaveData().sharedSave + enet::sSaveDataChunkSize * i, packet.size);
            sendPacket(&packet, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }

        mServer->getSaveDataMutex().unlock();
    }

    void Client::disconnect()
    {
    }

} // namespace enet
} // namespace pe
