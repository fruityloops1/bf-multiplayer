#include "pe/Enet/NetClient.h"
#include "al/Functor/FunctorV0M.h"
#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveFunction.h"
#include "enet/enet.h"
#include "enet/list.h"
#include "heap/seadHeapMgr.h"
#include "nn/os.h"
#include "pe/Enet/Channels.h"
#include "pe/Enet/Enet.h"
#include "pe/Enet/PacketHandler.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Account.h"
#include "pe/Util/Log.h"

namespace pe {
namespace enet {

    namespace {

        NERVE_DEF(NetClient, Stall);
        NERVE_DEF(NetClient, Connect);
        NERVE_DEF(NetClient, Service);

    } // namespace

    NetClient::NetClient(PacketHandler<void>* handler)
        : al::NerveExecutor("NetClient")
        , al::AsyncFunctorThread("NetClient", al::FunctorV0M(this, &NetClient::threadFunc), 1)
        , mPacketHandler(handler)
    {
        initNerve(&nrvNetClientStall);
    }

    void NetClient::threadFunc()
    {
        sead::ScopedCurrentHeapSetter setter(pe::enet::getEnetHeap());
        const nn::TimeSpan wait = nn::TimeSpan::FromMilliSeconds(sStepInterval);

        while (!mIsDead) {
            updateNerve();
            nn::os::SleepThread(wait);
        }
    }

    void NetClient::connect(const char* ip, u16 port)
    {
        mIp = ip;
        mPort = port;

        al::setNerve(this, &nrvNetClientConnect);
    }

    bool NetClient::isConnected() const { return al::isNerve(this, &nrvNetClientService); }

    void NetClient::disconnect()
    {
        mClientCS.lock();

        if (mServerPeer) {
            enet_peer_disconnect_now(mServerPeer, 0);
            mServerPeer = nullptr;
        }

        if (mClient) {
            enet_host_flush(mClient);
            enet_host_destroy(mClient);
            mClient = nullptr;
        }

        al::setNerve(this, &nrvNetClientStall);

        mClientCS.unlock();
    }

    void NetClient::sendPacket(const IPacket* packet, bool reliable)
    {
        if (mServerPeer == nullptr)
            return;

        if (mServerPeer->state != ENET_PEER_STATE_CONNECTED)
            return;
        size_t len = packet->calcSize();
        void* buf = getEnetHeap()->alloc(len);
        packet->build(buf);

        // ENET_PACKET_FLAG_NO_ALLOCATE is FUCKING broken dont use it
        mClientCS.lock();

        ENetPacket* pak = enet_packet_create(buf, len, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        ChannelType type = identifyType(packet);
        enet_peer_send(mServerPeer, (int)type, pak);
        mPacketHandler->increaseSentPacketCount(type);

        // enet_host_flush(mClient); // try this later

        mClientCS.unlock();
        getEnetHeap()->free(buf);
    }

    void NetClient::printStatusMsg(char* buf)
    {
        if (al::isNerve(this, &nrvNetClientStall))
            snprintf(buf, 256, "Stalling");
        else if (al::isNerve(this, &nrvNetClientConnect))
            snprintf(buf, 256, "Connecting to %s:%u", mIp.cstr(), mPort);
        else if (al::isNerve(this, &nrvNetClientService))
            snprintf(buf, 256, "Connected to %s:%u - Ping: %dms (avg %dms +- ~%dms) - Clock: %f %lu - Step: %d, Events handled in %dms: %d",
                mIp.cstr(), mPort, mServerPeer->lastRoundTripTime, mServerPeer->roundTripTime, mServerPeer->roundTripTimeVariance, getSyncClock(), getSyncClockFrames(), al::getNerveStep(this), sStepInterval, mPacketsHandledInCurrentTick);
    }

    void NetClient::printStatusMsgPretty(char* buf, float secondsElapsed)
    {
        if (al::isNerve(this, &nrvNetClientConnect))
            snprintf(buf, 256, "Connecting to %s:%u... %ds - Press B to cancel", mIp.cstr(), mPort, int(secondsElapsed));
        else if (al::isNerve(this, &nrvNetClientService))
            snprintf(buf, 256, "Successfully connected to %s:%u - Ping: %dms ",
                mIp.cstr(), mPort, mServerPeer->lastRoundTripTime);
    }

    void NetClient::updateSyncClockStartTick(u64 microseconds)
    {
        u64 ticks = microseconds * (double(nn::os::GetSystemTickFrequency()) / 1000000);
        mSyncClockStartTick = nn::os::GetSystemTick().m_tick - ticks;
    }

    s64 NetClient::getSyncClockTicks() const
    {
        return nn::os::GetSystemTick().m_tick - mSyncClockStartTick;
    }

    double NetClient::getSyncClock() const
    {
        return fmod(getSyncClockTicks() / double(nn::os::GetSystemTickFrequency()), 14400.0);
    }

    bool NetClient::checkStall()
    {
        if (mClient == nullptr) {
            al::setNerve(this, &nrvNetClientStall);
            return true;
        }
        return false;
    }

    void NetClient::exeStall()
    {
        if (al::isFirstStep(this))
            pe::warn("pe::enet::NetClient stalling");
    }

    void NetClient::exeConnect()
    {
        mClientCS.lock();

        if (al::isFirstStep(this)) {
            if (mClient)
                enet_host_destroy(mClient);

            mClient = enet_host_create(nullptr, 1, 2, 0, 0);
            if (mClient == nullptr) {
                pe::err("ENet client creation failed");
            }

            ENetAddress addr;
            enet_address_set_host_ip(&addr, mIp.cstr());
            addr.port = mPort;

            mServerPeer = enet_host_connect(mClient, &addr, sChannels.channelCount, 0);
            if (mServerPeer == nullptr) {
                pe::err("No available peers for initiating an ENet connection.");
                al::setNerve(this, &nrvNetClientStall);
                return;
            }
            pe::log("Attempting to connect to server at %s:%u...", mIp.cstr(), mPort);
        }

        if (checkStall()) {
            mClientCS.unlock();
            return;
        }

        ENetEvent event;
        while (enet_host_service(mClient, &event, 0) > 0)
            if (event.type == ENET_EVENT_TYPE_CONNECT)
                al::setNerve(this, &nrvNetClientService);

        if (al::getNerveStep(this) > 5 * 60)
            al::setNerve(this, &nrvNetClientConnect);
        mClientCS.unlock();
    }

    void NetClient::exeService()
    {
        if (al::isFirstStep(this)) {
            ToS_Hello greetPacket;
            greetPacket.isGhost = false;

            greetPacket.uid = util::getCurrentUserUid();
            if (greetPacket.uid.IsValid()) {
                nn::Result result = nn::account::GetNickname(&greetPacket.name, greetPacket.uid);
                if (result.IsFailure())
                    pe::err("Failed to get nn nickname %zu", result);
            }

            sendPacket(&greetPacket);
            pe::log("Connected to server at %s:%u", mIp.cstr(), mPort);
        }

        ENetEvent event;
        if (!mClientCS.tryLock())
            return;

        if (checkStall()) {
            mClientCS.unlock();
            return;
        }

        u32 maxPackets = sMaxPacketsPerStep;
        while (maxPackets-- && enet_host_service(mClient, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_DISCONNECT: {
                pe::log("Disconnected from server");
                // al::setNerve(this, &nrvNetClientStall);
                al::setNerve(this, &nrvNetClientConnect);
                mClientCS.unlock();
                return;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                if (event.channelID > pe::enet::sChannels.channelCount) {
                    pe::err("Invalid packet type %d", event.channelID);
                    break;
                }
                mPacketHandler->handlePacket((ChannelType)event.channelID, event.packet->data, event.packet->dataLength);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_CONNECT:
                MultiplayerManager::instance()->onConnect();
                break;
            case ENET_EVENT_TYPE_NONE:
            default:
                break;
            }
        }
        mPacketsHandledInCurrentTick = sMaxPacketsPerStep - maxPackets;

        mClientCS.unlock();
    }

    NetClient* getNetClient()
    {
        MultiplayerManager* mgr = MultiplayerManager::instance();
        return mgr ? mgr->getClient() : nullptr;
    }

} // namespace enet
} // namespace pe
