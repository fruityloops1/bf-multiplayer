#pragma once

#include "pe/Enet/ArrayPacket.h"
#include "pe/Enet/IPacket.h"

namespace pe {
namespace enet {

    struct PlayerListDataEntry {
        nn::account::Uid user {};
        nn::account::Nickname name { 0 };
        u8 playerType = 0;
        u8 playerFigure = 0;
        bool isNew = false;
        bool isGhost = false;
    };

    using ToC_PlayerList = ArrayPacket<PlayerListDataEntry>;

} // namespace enet
} // namespace pe
