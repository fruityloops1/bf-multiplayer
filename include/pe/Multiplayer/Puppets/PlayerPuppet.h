#pragma once

#include "Game/Layout/RCS_UserNameplate.h"
#include "Game/Player/PlayerAnimFunc.h"
#include "Game/Player/PlayerAnimator.h"
#include "Game/Player/PlayerModelHolder.h"
#include "al/LiveActor/LiveActor.h"
#include "pe/Multiplayer/Puppets/PuppetSensorActor.h"
#include "pe/Util/PlayerModelNameUtil.h"

namespace pe {
class PlayerData;
namespace puppets {

    void initPlayerBonkHook();

    class PlayerPuppet : public al::LiveActor {
        PlayerData* mPlayerData = nullptr;

    public:
        PlayerModelHolder* mModelHolder = nullptr;
        PlayerAnimator* mPlayerAnimator = nullptr;
        util::PlayerType mPlayerType = util::PlayerType::Mario;
        RCS_UserNameplate* mNameplate = nullptr;

        PlayerPuppet(util::PlayerType type);
        ~PlayerPuppet() override;

        void init(const al::ActorInitInfo& info) override;
        void initLayout(const al::LayoutInitInfo& info);
        void control() override;
        void appear() override;
        void kill() override;

        void change(EPlayerFigure figure);

        void startAnim(const sead::SafeString& anim);
        void startSubAnim(const sead::SafeString& anim);
        void startMaterialAnim(const sead::SafeString& anim);

        void attackSensor(al::HitSensor* me, al::HitSensor* other) override;
        bool receiveMsg(const al::SensorMsg* msg, al::HitSensor* other, al::HitSensor* me) override;

        void setPlayerData(PlayerData* data) { mPlayerData = data; }
    };

} // namespace puppets
} // namespace pe
