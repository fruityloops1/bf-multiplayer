#pragma once

#include "al/Nerve/NerveExecutor.h"
#include "enet/enet.h"
#include "pe/Enet/Client.h"
#include "pe/Enet/Console.h"
#include "pe/Enet/PacketHandler.h"
#include "pe/ServerSaveData.h"
#include "pe/ServerUtil.h"
#include <chrono>
#include <mutex>
#include <thread>
#include <type_traits>
#include <unordered_map>

namespace pe {
namespace enet {
    class Client;

    class Server : public al::NerveExecutor {
        bool mIsDead = false;
        std::thread mServerThread;
        std::chrono::high_resolution_clock::time_point mSyncClockStartTimestamp;
        const ENetCallbacks mCallbacks;
        ENetAddress mAddress {};
        ENetHost* mServer = nullptr;
        int mExitCode = 0;
        PacketHandler<Client>& mPacketHandler;
        std::unordered_map<size_t /* hash code of ENetAddress */, Client> mClients;
        Console mConsole;
        ServerSaveData mSaveData;
        std::recursive_mutex mEnetMutex;

        std::mutex mSaveDataMutex;
        Client* mCurrentSaveDataUploader = nullptr;
        bool mSaveDataAck[8] { false };
        size_t mSharedSaveWorkBufferSize = 0;
        uint8_t mSharedSaveWorkBuffer[std::numeric_limits<uint16_t>::max() + 0x100];

        void threadFunc();
        void fail(const char* msg);

    public:
        Server(ENetAddress address, PacketHandler<Client>& handler, const ENetCallbacks& callbacks);

        void exeStall();
        void exeStartup();
        void exeService();

        void sendPacketToAll(IPacket* packet, bool reliable = true);
        void sendPacketToAllExcept(Client* except, IPacket* packet, bool reliable = true);

        void start();
        int join()
        {
            if (mServerThread.joinable())
                mServerThread.join();
            if (mConsole.mThread.joinable())
                mConsole.mThread.join();
            return mExitCode;
        }

        void kill(int exitCode)
        {
            mIsDead = true;
            mExitCode = exitCode;
        }

        bool isAlive() const { return !mIsDead; }
        ServerSaveData& getSaveData() { return mSaveData; }
        std::mutex& getSaveDataMutex() { return mSaveDataMutex; }

        template <typename T, bool Reliable = true>
        static void redistributePacket(typename RedistPacket<T>::ServerBound* packet, Client* client)
        {
            typename RedistPacket<T>::ClientBound redistPacket(client->mUid, packet->getData());

            client->mServer->sendPacketToAllExcept(client, &redistPacket, Reliable);
        }

        friend class Console;
        friend class pe::ServerUtil;

        auto getSyncClockStartTimestamp() { return mSyncClockStartTimestamp; }
        void flush() { enet_host_flush(mServer); }

        Client* searchClient(const char* query);

        static constexpr int sSyncClockInterval = 12000;
        friend struct Handlers;
    };

} // namespace enet
} // namespace pe
