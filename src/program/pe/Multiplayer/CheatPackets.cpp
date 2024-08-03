#include "pe/Multiplayer/CheatPackets.h"
#include "Game/Scene/SingleModeScene.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Nerve.h"

namespace pe {

void handleToC_CheatPacket(enet::ToC_CheatPacket* packet)
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr) {
        pe::warn("CheatPacket but no mgr");
    }

    // clang-format off
    ProductSequence* sequence = mgr->getSequence();
    if (sequence == nullptr or !util::isSequenceSingleModeGameOnlyNrv(sequence))
    {
        pe::warn("CheatPacket but wrong nerve");
        return;
    }
    // clang-format on

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(sequence->tryGetCurrentScene());
    if (scene == nullptr) {
        pe::warn("CheatPacket but no scene");
        return;
    }

    PlayerActor* player = mgr->getPlayerActor();

    if (packet->type == enet::ToC_CheatPacket::Type::Teleport) {
        const sead::Vector3f& trans = packet->teleport.trans;
        al::setTrans(player, trans);
        player->mPlayer->mPlayerProperty->trans = trans;
    }
}

} // namespace pe
