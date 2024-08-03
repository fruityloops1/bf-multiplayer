#pragma once

#include "al/LiveActor/LiveActor.h"

namespace pe {
class PlayerData;
namespace puppets {
    class RaidonPuppet : public al::LiveActor {
        PlayerData* mPlayerData = nullptr;

    public:
        RaidonPuppet();
        ~RaidonPuppet() override;

        void init(const al::ActorInitInfo& info) override;
        void appear() override;
        void kill() override;

        void setPlayerData(PlayerData* data) { mPlayerData = data; }
    };

} // namespace puppets
} // namespace pe
