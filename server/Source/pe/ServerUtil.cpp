#include "pe/ServerUtil.h"
#include "pe/Enet/Client.h"
#include "pe/Enet/Server.h"

namespace pe {

enet::ToC_PlayerList ServerUtil::makePlayerListPacket(enet::Server* server, enet::Client* newClient)
{
    int numClients = 0;

    for (auto& entry : server->mClients) {
        if (!entry.second.mIsGhost)
            numClients++;
    }

    enet::ToC_PlayerList packet(numClients);

    {
        int i = 0;
        for (auto& entry : server->mClients) {
            if (entry.second.mIsGhost)
                continue;
            packet[i]->user = entry.second.mUid;
            packet[i]->playerFigure = entry.second.mPlayerFigure;
            packet[i]->playerType = entry.second.mPlayerType;
            packet[i]->isNew = &entry.second == newClient;
            std::memcpy(packet[i]->name, entry.second.mName, sizeof(nn::account::Uid));
            i++;
        }
    }

    return packet;
}

} // namespace pe
