#include "pe/Enet/Server.h"
#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveUtil.h"
#include "enet/enet.h"
#include "pe/Enet/Channels.h"
#include "pe/Enet/Client.h"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <mutex>
#include <thread>

namespace pe {
namespace enet {

    namespace {
        NERVE_DEF(Server, Stall)
        NERVE_DEF(Server, Startup)
        NERVE_DEF(Server, Service)
    }

    Server::Server(ENetAddress address, PacketHandler<Client>& handler, const ENetCallbacks& callbacks)
        : al::NerveExecutor("Server")
        , mCallbacks(callbacks)
        , mAddress(address)
        , mPacketHandler(handler)
        , mConsole(this)
    {
        initNerve(&nrvServerStall);
        mSyncClockStartTimestamp = std::chrono::high_resolution_clock::now();
        mSaveData.load();
    }

    void Server::threadFunc()
    {
        while (!mIsDead)
            updateNerve();
    }

    void Server::fail(const char* msg)
    {
        fprintf(stderr, "%s\n", msg);
        kill(1);
    }

    void Server::sendPacketToAll(IPacket* packet, bool reliable)
    {
        size_t len = packet->calcSize();
        u8* buf = new u8[len];
        packet->build(buf);

        mEnetMutex.lock();
        ENetPacket* pak = enet_packet_create(buf, len, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        ChannelType channel = identifyType(packet);
        for (int i = 0; i < mServer->peerCount; i++)
            mPacketHandler.increaseSentPacketCount(channel);

        enet_host_broadcast(mServer, (int)channel, pak);
        mEnetMutex.unlock();

        delete[] buf;
    }

    void Server::sendPacketToAllExcept(Client* except, IPacket* packet, bool reliable)
    {
        size_t len = packet->calcSize();
        u8* buf = new u8[len];
        packet->build(buf);

        mEnetMutex.lock();
        ENetPacket* pak = enet_packet_create(buf, len, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        for (auto& entry : mClients)
            if (&entry.second != except) {
                ChannelType channel = identifyType(packet);
                mPacketHandler.increaseSentPacketCount(channel);
                enet_peer_send(entry.second.getPeer(), (int)channel, pak);
            }
        mEnetMutex.unlock();

        delete[] buf;
    }

    void Server::start()
    {
        al::setNerve(this, &nrvServerStartup);
        mServerThread = std::thread(&Server::threadFunc, this);
        mConsole.start();
    }

    void Server::exeStall() { Console::log("Server is Stalling"); }

    void Server::exeStartup()
    {
        if (al::isFirstStep(this)) {

            mEnetMutex.lock();
            if (enet_initialize_with_callbacks(ENET_VERSION, &mCallbacks) != 0) {
                fail("An error occurred while initializing ENet.");
                return;
            }

            mServer = enet_host_create(&mAddress, 12, pe::enet::sChannels.channelCount, 0, 0);
            if (mServer == nullptr) {
                fail("An error occurred while creating ENet server host.");
                return;
            }

            mEnetMutex.unlock();
            al::setNerve(this, &nrvServerService);
        }
    }

    void Server::exeService()
    {
        if (al::isFirstStep(this)) {
            Console::log("Server initialized");
        }

        if (mServer == nullptr) {
            al::setNerve(this, &nrvServerStall);
            return;
        }

        mEnetMutex.lock();

        ENetEvent event;
        while (enet_host_service(mServer, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                ENetAddress addr = event.peer->address;
                size_t hash = calcEnetAddressHashCode(addr);

                if (mClients.contains(hash)) {
                    Console::log("Warning: ghost client removed");
                    mClients.erase(hash);
                }

                mClients[hash] = Client(this, event.peer);
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE: {
                ENetAddress addr = event.peer->address;
                size_t hash = calcEnetAddressHashCode(addr);

                if (!mClients.contains(hash)) {
                    printf("Warning: packet from invalid peer. disconnecting");
                    enet_peer_disconnect(event.peer, 0);
                    break;
                }

                Client& client = mClients[hash];
                ChannelType packetType = (ChannelType)event.channelID;
                if (packetType != ChannelType::ToS_Hello && !client.hasGreeted()) {
                    Console::log("Warning: client %zx tried to send packet without greeting", calcEnetAddressHashCode(addr));
                    enet_packet_destroy(event.packet);
                    break;
                }

                mPacketHandler.handlePacket(packetType, event.packet->data, event.packet->dataLength, &client);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT: {
                ENetAddress addr = event.peer->address;
                size_t hash = calcEnetAddressHashCode(addr);

                Console::log("Client %zx disconnected", calcEnetAddressHashCode(addr));

                if (mClients.contains(hash)) {
                    {
                        Client& client = mClients[hash];
                        client.disconnect();
                    }
                    mClients.erase(hash);
                    ToC_PlayerList listPacket = ServerUtil::makePlayerListPacket(this);
                    sendPacketToAll(&listPacket);
                }
                break;
            }
            case ENET_EVENT_TYPE_NONE:
            default:
                break;
            }
        }

        mEnetMutex.unlock();
    }

    Client* Server::searchClient(const char* query)
    {
        for (auto it = mClients.begin(); it != mClients.end(); ++it) {
            Client* client = &it->second;
            if (strcmp(client->mName, query) == 0)
                return client;
            if (std::string(client->mName).find(query) != std::string::npos)
                return client;
        }
        return nullptr;
    }

} // namespace enet
} // namespace pe
