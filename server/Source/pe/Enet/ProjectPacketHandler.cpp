#include "pe/Enet/ProjectPacketHandler.h"
#include "main.h"
#include "pe/Enet/Channels.h"
#include "pe/Enet/Console.h"
#include "pe/Enet/PacketHandler.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/Server.h"
#include "pe/ServerSaveData.h"
#include "pe/Util.h"
#include <chrono>
#include <ratio>
#include <thread>

namespace pe {
namespace enet {

    struct Handlers {
        static void handlePacketOnNewThread(ProjectPacketHandler* handler, ChannelType type, const void* data, size_t len, Client* client)
        {
            handler->PacketHandler<Client>::handlePacket(type, data, len, client);
            buddyFree((void*)data);
        }

        static void handleGreet(ToS_Hello* packet, Client* client)
        {
            client->handleGreet(packet);
        }

        static void handleLog(ToS_Log* packet, Client* client)
        {
            const char* prefix = "[LOG:";
            switch (packet->getType()) {
            case 1:
                prefix = "\033[0;33m[WARN:";
                break;
            case 2:
                prefix = "\033[1;31m[ERROR:";
                break;
            default:
                break;
            }
            Console::log("%s%s] %s\033[0m", prefix, client->mName, packet->getMsg());
        }

        static void handleSingleModeSceneEnter(ToS_SingleModeSceneEnter* packet, Client* client)
        {
            client->mPlayerType = packet->playerType;

            // need to refresh puppets
            ToC_PlayerList listPacket = ServerUtil::makePlayerListPacket(client->mServer);
            client->mServer->sendPacketToAll(&listPacket);

            const auto& saveData = client->mServer->getSaveData();
            for (const auto& island : saveData.islandToScenarioMap) {
                ScenarioUpdateData data;
                strncpy(data.islandName, island.first.c_str(), sizeof(data.islandName));
                data.scenario = island.second;
                ToC_ScenarioUpdate packet(nn::account::Uid(), data);

                client->sendPacket(&packet, true);
                client->mServer->flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }

            for (const auto& shine : saveData.collectedGoalItems) {
                ToC_GoalItemInfo packet;
                packet.numCollectedGoalItems = saveData.numCollectedGoalItems;
                packet.isCollectNewShine = false;
                packet.islandID = shine.islandID;
                packet.shineID = shine.scenarioID;

                client->sendPacket(&packet, true);
                client->mServer->flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }

            for (int toad : saveData.unlockedToads) {
                MiscMapSyncData data;
                data.type = MiscMapSyncData::Type::ToadUnlock;
                data.toadUnlock.character = toad;
                ToC_MiscMapSync packet(nn::account::Uid(), data);

                client->sendPacket(&packet, true);
                client->mServer->flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }

            for (const auto& shard : saveData.collectedShards) {
                MiscMapSyncData data;
                data.type = MiscMapSyncData::Type::ShardCollect;
                data.shardCollect.shardID = shard.scenarioID;
                data.shardCollect.zoneID = shard.islandID;
                ToC_MiscMapSync packet(nn::account::Uid(), data);

                client->sendPacket(&packet, true);
                client->mServer->flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }

        static void handleGoalItemInfo(ToS_GoalItemInfo* packet, Client* client)
        {
            auto& saveData = client->mServer->getSaveData();
            bool isAlreadyCollected = saveData.isGoalItemAlreadyCollected(packet->islandID, packet->shineID);

            ToC_GoalItemInfo distPacket;
            distPacket.user = client->mUid;
            distPacket.isCollectNewShine = !isAlreadyCollected;

            if (!isAlreadyCollected) {
                saveData.numCollectedGoalItems++;
                saveData.collectedGoalItems.push_back({ packet->islandID, packet->shineID });
                distPacket.islandID = packet->islandID;
                distPacket.shineID = packet->shineID;
            }
            distPacket.numCollectedGoalItems = saveData.numCollectedGoalItems;

            client->mServer->sendPacketToAllExcept(client, &distPacket);

            distPacket.isCollectNewShine = false;
            distPacket.numCollectedGoalItems = saveData.numCollectedGoalItems;
            client->sendPacket(&distPacket);

            client->mServer->getSaveData().save();
        }

        static void handleScenarioUpdate(ToS_ScenarioUpdate* packet, Client* client)
        {
            Server::redistributePacket<ScenarioUpdateData>(packet, client);

            std::string islandName(packet->getData().islandName);
            auto& saveData = client->mServer->getSaveData();
            saveData.islandToScenarioMap[islandName] = packet->getData().scenario;
        }

        static void handleJumpFlip(ToS_JumpFlip* packet, Client* client)
        {

            static std::chrono::high_resolution_clock::time_point lastFlip = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            std::chrono::milliseconds timeSinceLastFlip(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFlip));
            if (timeSinceLastFlip.count() < 16)
                return;
            lastFlip = now;

            auto& saveData = client->mServer->getSaveData();
            saveData.jumpFlipState = !saveData.jumpFlipState;

            JumpFlipData data { saveData.jumpFlipState };
            ToC_JumpFlip redistPacket(client->mUid, data);
            client->mServer->sendPacketToAllExcept(client, &redistPacket, false);
            if (packet->getData().flipState != saveData.jumpFlipState)
                client->sendPacket(&redistPacket, false);
        }

        static void handleMiscMapSync(ToS_MiscMapSync* packet, Client* client)
        {
            Server::redistributePacket<MiscMapSyncData>(packet, client);
            auto& saveData = client->mServer->getSaveData();

            if (packet->getData().type == MiscMapSyncData::Type::ToadUnlock) {
                for (int toad : saveData.unlockedToads)
                    if (toad == packet->getData().toadUnlock.character)
                        return;

                saveData.unlockedToads.push_back(packet->getData().toadUnlock.character);
                saveData.save();
            } else if (packet->getData().type == MiscMapSyncData::Type::ShardCollect) {
                ServerSaveData::GoalItemCollectData data;
                data.islandID = packet->getData().shardCollect.zoneID;
                data.scenarioID = packet->getData().shardCollect.shardID;

                saveData.collectedShards.push_back(data);
                saveData.save();
            }
        }

        static void handlePuppetPose(ToS_PuppetPose* packet, Client* client)
        {
            Server::redistributePacket<PuppetPoseData, false>(packet, client);
            client->mTrans = packet->mPacketData.trans;
        }

        static void handleSaveChunk(SaveDataChunk* packet, Client* client)
        {
            Server* server = client->mServer;
            bool isEmpty = true;
            for (int i = 0; i < 8; i++) {
                if (server->mSaveDataAck[i])
                    isEmpty = false;
            }

            if (isEmpty) {
                server->mSharedSaveWorkBufferSize = 0;
                server->mCurrentSaveDataUploader = client;
            }

            if (server->mCurrentSaveDataUploader != client)
                return;
            if (packet->chunkIndex < 0 || packet->chunkIndex >= 8)
                return;
            if (packet->size > sSaveDataChunkSize)
                return;

            memcpy(server->mSharedSaveWorkBuffer + sSaveDataChunkSize * packet->chunkIndex, packet->data, packet->size);
            if (server->mSaveDataAck[packet->chunkIndex] == false) {
                server->mSharedSaveWorkBufferSize += packet->size;
                server->mSaveDataAck[packet->chunkIndex] = true;
            }

            bool isFull = true;
            for (int i = 0; i < 8; i++) {
                if (server->mSaveDataAck[i] == false)
                    isFull = false;
            }

            if (isFull) {
                memset(server->mSaveDataAck, 0, sizeof(server->mSaveDataAck));
                server->mCurrentSaveDataUploader = nullptr;

                server->mSaveDataMutex.lock();

                server->getSaveData().sharedSaveSize = server->mSharedSaveWorkBufferSize;
                memcpy(server->getSaveData().sharedSave, server->mSharedSaveWorkBuffer, server->mSharedSaveWorkBufferSize);
                server->getSaveData().save();

                server->mSaveDataMutex.unlock();
            }
        }
    };

    const static PacketHandler<Client>::PacketHandlerEntry sEntries[] {
        { ChannelType::ToS_Hello,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleGreet },
        { ChannelType::ToS_Log,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleLog },
        { ChannelType::ToS_SingleModeSceneEnter,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleSingleModeSceneEnter, true },
        { ChannelType::ToS_PuppetPose,
            (PacketHandler<Client>::HandlePacketType)Handlers::handlePuppetPose },
        { ChannelType::ToS_Action,
            (PacketHandler<Client>::HandlePacketType)Server::redistributePacket<ActionPacketData, false> },
        { ChannelType::ToS_PlayerFigureChange,
            (PacketHandler<Client>::HandlePacketType)Server::redistributePacket<PlayerFigureChangeData> },
        { ChannelType::ToS_GoalItemInfo,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleGoalItemInfo },
        { ChannelType::ToS_PhaseUnlock,
            (PacketHandler<Client>::HandlePacketType)Server::redistributePacket<PhaseUnlockData> },
        { ChannelType::ToS_ScenarioUpdate,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleScenarioUpdate },
        { ChannelType::ToS_DisasterModeControllerInternalUpdate,
            (PacketHandler<Client>::HandlePacketType)Server::redistributePacket<DisasterModeControllerInternalUpdateData> },
        { ChannelType::ToS_MiscMapSync,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleMiscMapSync },
        { ChannelType::ToS_JumpFlip,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleJumpFlip },
        { ChannelType::SaveDataChunk,
            (PacketHandler<Client>::HandlePacketType)Handlers::handleSaveChunk },
    };

    ProjectPacketHandler::ProjectPacketHandler()
        : PacketHandler(sEntries)
    {
    }

    void ProjectPacketHandler::handlePacket(ChannelType type, const void* data, size_t len, Client* client)
    {
        for (int i = 0; i < mNumEntries; i++) {
            const PacketHandlerEntry& entry = mEntries[i];
            if (entry.type == type) {
                if (entry.useSeparateThread) {
                    void* clone = buddyMalloc(len);
                    memcpy(clone, data, len);
                    std::thread packetThread(Handlers::handlePacketOnNewThread, this, type, clone, len, client);
                    packetThread.detach();
                } else
                    PacketHandler<Client>::handlePacket(type, data, len, client);
                return;
            }
        }

        PENET_ABORT("Packet Handler for packet %hhu not found\n", type);
    }

} // namespace enet
} // namespace pe
