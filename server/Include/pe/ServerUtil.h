#pragma once

#include "pe/Enet/Packets/ToC_PlayerList.h"

namespace pe {

namespace enet {
    class Server;
    class Client;
} // namespace enet

class ServerUtil {
public:
    static enet::ToC_PlayerList makePlayerListPacket(enet::Server* server, enet::Client* newClient = nullptr);
};

} // namespace pe
