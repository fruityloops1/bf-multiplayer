#include "pe/Multiplayer/Puppets/PlayerPuppet.h"
#include "Game/Layout/RCS_UserNameplate.h"
#include "Game/Player/PlayerModelHolderCreator.h"
#include "Game/Player/PlayerRetargettingSelectorSceneObj.h"
#include "Game/Util/SensorUtil.h"
#include "al/HitSensor/HitSensor.h"
#include "al/HitSensor/SensorMsg.h"
#include "al/LiveActor/ActorClippingFunction.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "al/LiveActor/ActorSensorFunction.h"
#include "pe/Multiplayer/PlayerData.h"
#include "pe/Multiplayer/Puppets/PuppetSensorActor.h"
#include "pe/Util/Log.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/Type.h"

namespace pe {
namespace puppets {

    void initPlayerBonkHook()
    {
    }

    PlayerPuppet::PlayerPuppet(util::PlayerType type)
        : al::LiveActor("PlayerPuppet")
        , mPlayerType(type)
    {
    }

    PlayerPuppet::~PlayerPuppet()
    {
        if (mPlayerData)
            mPlayerData->setPuppet(nullptr);
    }

    void PlayerPuppet::init(const al::ActorInitInfo& info)
    {
        al::initActorWithArchiveName(this, info, "PlayerPuppet");
        al::initActorModelKeeper(this, info, "ObjectData/Mario", 6, "ObjectData/MarioAnimation");

        PlayerRetargettingSelectorSceneObj* retargettingSelector = static_cast<PlayerRetargettingSelectorSceneObj*>(al::getSceneObj(this, 0x19));
        mModelHolder = PlayerModelHolderCreator::createModelHolderAll(info, util::getPlayerName(mPlayerType), reinterpret_cast<IUsePlayerRetargettingInfoCreator*>(retargettingSelector), this, true, PlayerModelHolderCreator::CreationFlag(), "SM", 0);
        mPlayerAnimator = new PlayerAnimator(this, mModelHolder, retargettingSelector);
        mPlayerAnimator->init();

        for (int i = 0; i < mModelHolder->mBufferSize; i++) {
            PlayerModel* model = mModelHolder->mModels[i];
            // this only causes crashes, there is no "real" player bound to player puppets, so we're getting rid of this one
            if (model)
                model->getActorActionKeeper()->mScreenEffectCtrl = nullptr;
        }
    }

    void PlayerPuppet::initLayout(const al::LayoutInitInfo& info)
    {
        mNameplate = new RCS_UserNameplate(this, info);
        mNameplate->init("Placeholder", false, false);
    }

    void PlayerPuppet::control()
    {
        mPlayerAnimator->update();
        if (mPlayerAnimator->mIsSubAnim && mPlayerAnimator->isSubAnimEnd())
            mPlayerAnimator->endSubAnim();
    }

    void PlayerPuppet::appear()
    {
        if (mNameplate)
            mNameplate->appear();
        mModelHolder->mModels[mModelHolder->mCurModelIndex]->appear();
        LiveActor::appear();
    }

    void PlayerPuppet::kill()
    {
        if (mNameplate)
            mNameplate->kill();
        for (int i = 0; i < 10; i++)
            mModelHolder->mModels[i]->kill();
        LiveActor::kill();
    }

    void PlayerPuppet::attackSensor(al::HitSensor* me, al::HitSensor* other)
    {
        if (al::isSensorMultiPlayer(me) && al::isSensorPlayer(other) && !al::sendMsgPush(other, me))
            rc::sendMsgPushConnected(other, me);

        if (al::isSensorMultiPlayer(me) && al::isSensorPlayer(other) && al::isEqualString(mPlayerAnimator->getAnimName(), "HipDrop")) {
            al::sendMsgPlayerHipDropKnockDown(other, me);
        }
    }

    bool PlayerPuppet::receiveMsg(const al::SensorMsg* msg, al::HitSensor* other, al::HitSensor* me)
    {
        if (al::isSensorPlayer(other) && al::isMsgPlayerTrampleForCrossoverSensor(msg, other, me)) {
            return true;
        }
        return false;
    }

    void PlayerPuppet::change(EPlayerFigure figure)
    {
        mModelHolder->change((int)figure);
    }

    void PlayerPuppet::startAnim(const sead::SafeString& anim)
    {
        mPlayerAnimator->startAnim(anim);
    }

    void PlayerPuppet::startSubAnim(const sead::SafeString& anim)
    {
        mPlayerAnimator->startSubAnim(anim);
    }

    void PlayerPuppet::startMaterialAnim(const sead::SafeString& anim)
    {
        mPlayerAnimator->startMaterialAnim(anim);
    }

} // namespace puppets
} // namespace pe
