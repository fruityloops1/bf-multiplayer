#include "pe/Enet/ProjectPacketHandler.h"
#include "pe/Enet/Channels.h"
#include "pe/Enet/Enet.h"
#include "pe/Enet/NetClient.h"
#include "pe/Enet/PacketHandler.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/Packets/ToC_PlayerList.h"
#include "pe/Multiplayer/CheatPackets.h"
#include "pe/Multiplayer/DisasterSync.h"
#include "pe/Multiplayer/GoalItemSync.h"
#include "pe/Multiplayer/MiscMapSync.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Multiplayer/SaveDataSync.h"

namespace pe {
namespace enet {

    static void handleToC_PlayerList(ToC_PlayerList* packet)
    {
        MultiplayerManager::instance()->handlePlayerListPacket(*packet);
    }

    static void handleToC_PuppetPose(ToC_PuppetPose* packet)
    {
        MultiplayerManager::instance()->handlePuppetPosePacket(*packet);
    }

    static void handleToC_Action(ToC_Action* packet)
    {
        MultiplayerManager::instance()->handleActionPacket(*packet);
    }

    static void handleToC_PlayerFigureChange(ToC_PlayerFigureChange* packet)
    {
        MultiplayerManager::instance()->handleFigurePacket(*packet);
    }

    static void handleToC_SyncClockUpdate(ToC_SyncClockUpdate* packet)
    {
        u64 progress = packet->microseconds;
        auto* client = getNetClient();
        progress += client->getPeer()->lastRoundTripTime * 500 /* x 1000 / 2 */;
        client->updateSyncClockStartTick(progress);
    }

    const static PacketHandler<void>::PacketHandlerEntry sEntries[] {
        { ChannelType::ToC_PlayerList,
            (PacketHandler<void>::HandlePacketType)handleToC_PlayerList },
        { ChannelType::ToC_PuppetPose,
            (PacketHandler<void>::HandlePacketType)handleToC_PuppetPose },
        { ChannelType::ToC_Action,
            (PacketHandler<void>::HandlePacketType)handleToC_Action },
        { ChannelType::ToC_PlayerFigureChange,
            (PacketHandler<void>::HandlePacketType)handleToC_PlayerFigureChange },
        { ChannelType::ToC_GoalItemInfo,
            (PacketHandler<void>::HandlePacketType)handleToC_GoalItemInfo },
        { ChannelType::ToC_PhaseUnlock,
            (PacketHandler<void>::HandlePacketType)handleToC_PhaseUnlock },
        { ChannelType::ToC_ScenarioUpdate,
            (PacketHandler<void>::HandlePacketType)handleToC_ScenarioUpdate },
        { ChannelType::ToC_DisasterModeControllerInternalUpdate,
            (PacketHandler<void>::HandlePacketType)handleToC_DisasterModeControllerInternalUpdate },
        { ChannelType::ToC_SyncClockUpdate,
            (PacketHandler<void>::HandlePacketType)handleToC_SyncClockUpdate },
        { ChannelType::ToC_MiscMapSync,
            (PacketHandler<void>::HandlePacketType)handleToC_MiscMapSync },
        { ChannelType::ToC_JumpFlip,
            (PacketHandler<void>::HandlePacketType)handleToC_JumpFlip },
        { ChannelType::ToC_CheatPacket,
            (PacketHandler<void>::HandlePacketType)handleToC_CheatPacket },
        { ChannelType::SaveDataChunk,
            (PacketHandler<void>::HandlePacketType)handle_SaveDataChunk },
    };

    ProjectPacketHandler::ProjectPacketHandler()
        : PacketHandler(sEntries)
    {
    }

} // namespace enet
} // namespace pe
