#pragma once

#include "al/Nerve/NerveExecutor.h"
#include "al/Thread/AsyncFunctorThread.h"
#include "enet/enet.h"
#include "prim/seadSafeString.h"
#include <sead/thread/seadCriticalSection.h>

namespace pe {
namespace enet {
    template <typename ClientType>
    class PacketHandler;
    class IPacket;

    class NetClient : public al::NerveExecutor, public al::AsyncFunctorThread {
        bool mIsDead = false;
        ENetHost* mClient = nullptr;
        sead::CriticalSection mClientCS;
        ENetPeer* mServerPeer = nullptr;
        sead::FixedSafeString<16> mIp;
        u16 mPort = 0;
        PacketHandler<void>* mPacketHandler = nullptr;
        s64 mSyncClockStartTick = 0;
        int mPacketsHandledInCurrentTick = 0;

        void threadFunc();

    public:
        NetClient(PacketHandler<void>* handler);
        void kill() { mIsDead = true; }

        void connect(const char* ip, u16 port);
        bool isConnected() const;
        void disconnect();

        void sendPacket(const IPacket* packet, bool reliable = true);

        void flush()
        {
            if (mClient)
                enet_host_flush(mClient);
        }

        PacketHandler<void>* getPacketHandler() const { return mPacketHandler; }
        ENetPeer* getPeer() const { return mServerPeer; }
        void printStatusMsg(char* buf);
        void printStatusMsgPretty(char* buf, float secondsElapsed);
        void updateSyncClockStartTick(u64 microseconds);
        s64 getSyncClockTicks() const;
        double getSyncClock() const;

        void exeStall();
        void exeConnect();
        void exeService();

        bool checkStall();

        constexpr static int sMaxPacketsPerStep = 32;
        constexpr static int sStepInterval = 48;
    };

    NetClient* getNetClient();
    inline s64 getSyncClockTicks() { return getNetClient()->getSyncClockTicks(); }
    inline double getSyncClock() { return getNetClient()->getSyncClock(); }
    inline u64 getSyncClockFrames() { return getSyncClock() * 60.0; }

} // namespace enet
} // namespace pe
