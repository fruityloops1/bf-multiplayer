#include "pe/Multiplayer/PlayerData.h"
#include "Game/MapObj/DisasterModeController.h"
#include "Game/Player/PlayerAnimFunc.h"
#include "Game/Sequence/ProductStateSingleMode.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/SceneObjUtil.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorModelFunction.h"
#include "al/LiveActor/ActorPoseKeeperBase.h"
#include "al/LiveActor/LiveActorFlag.h"
#include "al/Math/MathLerpUtil.h"
#include "al/Math/MathQuatUtil.h"
#include "al/Nerve/NerveFunction.h"
#include "hk/hook/InstrUtil.h"
#include "hk/ro/RoUtil.h"
#include "math/seadQuatCalcCommon.h"
#include "pe/Enet/Packets/Action.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Enet/Packets/PuppetPose.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Log.h"
#include "pe/Util/Math.h"

namespace pe {

static bool sIsSurfingRaidon = false;

template <bool IsSurfing>
static void raidonHook(al::IUseNerve* t, al::Nerve* nerve)
{
    sIsSurfingRaidon = IsSurfing;
    al::setNerve(t, nerve);
}

PlayerData::PlayerData(enet::PlayerListDataEntry packetData)
    : mListData(packetData)
{
    mActionQueue.allocBuffer(32, nullptr);

    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003bd0a0, raidonHook<true>);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003bd494, raidonHook<false>);
}

void PlayerData::update(MultiplayerManager* mgr)
{
    if (mPlayerPuppet) {
        PlayerModel* curModel = mPlayerPuppet->mModelHolder->getCurrentModel();
        if (al::isDead(mPlayerPuppet) || al::isDead(curModel)) {
            mPlayerPuppet->appear();
            curModel->appear();
            mPlayerPuppet->startAnim("Wait");
        }
        if (mPoseData.isSurfingRaidon) {
            if (al::isDead(mRaidonPuppet))
                mRaidonPuppet->appear();
        } else {
            if (!al::isDead(mRaidonPuppet))
                mRaidonPuppet->kill();
        }
        sead::Vector3f trans = al::getTrans(curModel);
        sead::Quatf quat = al::getQuat(curModel);

        s64 localTick = nn::os::GetSystemTick().m_tick - mLastPoseUpdateTimestamp;
        float progress = float(localTick) / mPoseUpdateIntervalTicks;
        if (progress > 1.0f)
            progress = 1.0f;

        al::lerpVec(&trans, mLastTrans, mPoseData.trans, progress);
        sead::QuatCalcCommon<float>::slerpTo(quat, mLastQuat, mPoseData.quat, progress);

        float animRate = al::lerpValue(mLastAnimRate, mPoseData.animRate, progress);
        float bw[6]; // blend weights
        for (int i = 0; i < 6; i++)
            bw[i] = al::lerpValue(mLastBlendWeights[i], mPoseData.blendWeights[i], progress);

        if (mPoseData.isSurfingRaidon) {
            al::setTrans(mRaidonPuppet, trans);
            al::updatePoseQuat(mRaidonPuppet, quat);
            al::calcJointPos(al::getTransPtr(curModel), mRaidonPuppet, "Player1");
            al::calcJointPos(al::getTransPtr(mPlayerPuppet), mRaidonPuppet, "Player1");
        } else {
            al::setTrans(curModel, trans);
            al::setTrans(mPlayerPuppet, trans);
        }

        al::updatePoseQuat(curModel, quat);
        al::updatePoseQuat(mPlayerPuppet, quat);

        MultiplayerManager::setStartAnimHookEnabled(false);
        mPlayerPuppet->mPlayerAnimator->setAnimRate(animRate);
        if (isBlendedAnim(mPlayerPuppet->mPlayerAnimator->mCurAnimName)) {
            mPlayerPuppet->mPlayerAnimator->setWeightSixfold(bw[0], bw[1], bw[2], bw[3], bw[4], bw[5]);
        }

        for (int i = 0; i < mActionQueue.size(); i++) {
            auto* entry = mActionQueue[i];
            entry->framesLeft--;
            if (entry->framesLeft == 0) {
                if (entry->data.type == enet::ActionPacketData::Type::Anim)
                    mPlayerPuppet->startAnim(entry->data.action);
                else if (entry->data.type == enet::ActionPacketData::Type::SubAnim)
                    mPlayerPuppet->startSubAnim(entry->data.action);
                else if (entry->data.type == enet::ActionPacketData::Type::MaterialAnim)
                    mPlayerPuppet->startMaterialAnim(entry->data.action);
                else
                    al::startAction(mRaidonPuppet, entry->data.action);
                mActionQueue.erase(i);
                delete entry;
            }
        }

        MultiplayerManager::setStartAnimHookEnabled(true);

        if (mPlayerPuppet->mNameplate)
            mPlayerPuppet->mNameplate->init(mListData.name, false, false);

        float scale = rc::isPlayerGiga(mgr->mPlayerActor) ? 30 : 1;

        al::setScale(mPlayerPuppet, scale);
        al::setScale(mPlayerPuppet->mModelHolder->getCurrentModel(), scale);
    }
    mFrameCount++;
}

void PlayerData::updateLocal(MultiplayerManager* mgr)
{
    if (mgr->mPlayerActor == nullptr)
        return;
    bool isRaidon = sIsSurfingRaidon && mgr->mRaidonSurf;

    const sead::Vector3f& trans = isRaidon ? al::getTrans(mgr->mRaidonSurf) : al::getTrans(mgr->mPlayerActor);
    const sead::Vector3f delta = abs(mLastTrans - trans);
    const sead::Vector3f& rotate = al::getRotate(mgr->mPlayerActor);
    const sead::Vector3f rotateDelta = abs(mLastRotate - rotate);

    bool moving = delta.x > 1 or delta.y > 1 or delta.z > 1 or rotateDelta.x > 1 or rotateDelta.y > 1 or rotateDelta.z > 1;
    bool interval = mFrameCount % MultiplayerManager::sPosePacketInterval == 0;

    if ((moving && interval) || mFramesWithZeroDelta == 3) {
        enet::PuppetPoseData data;
        data.trans = trans;
        al::calcQuat(&data.quat, isRaidon ? (al::LiveActor*)mgr->mRaidonSurf : mgr->mPlayerActor);
        data.animRate = al::getSklAnimFrameRate(mgr->mPlayerActor->mPlayerAnimator->mModelHolder->getCurrentModel(), 0);
        std::memcpy(data.blendWeights, mgr->mPlayerActor->mPlayerAnimator->mBlendWeights, sizeof(data.blendWeights));
        data.isSurfingRaidon = sIsSurfingRaidon;

        enet::ToS_PuppetPose packet(data);
        mgr->mNetClient->sendPacket(&packet, false);
    }
    if (!moving)
        mFramesWithZeroDelta++;
    else
        mFramesWithZeroDelta = 0;
    mLastTrans = al::getTrans(isRaidon ? (al::LiveActor*)mgr->mRaidonSurf : mgr->mPlayerActor);
    mLastRotate = isRaidon ? pe::quatToRotate(al::getQuat(mgr->mRaidonSurf)) : al::getRotate(mgr->mPlayerActor);

    EPlayerFigure figure = mgr->mPlayerActor->mPlayer->mFigureDirector->mCurFigure;

    if (figure != mLastFigure) {
        enet::ToS_PlayerFigureChange packet({ (u8)figure });
        mgr->mNetClient->sendPacket(&packet);
    }
    mLastFigure = figure;

    if (isRaidon) {
        const char* raidonAction = al::getActionName(mgr->mRaidonSurf);
        if (!al::isEqualString(mLastRaidonAction, raidonAction)) {
            enet::ActionPacketData data;
            std::strncpy(data.action, raidonAction, sizeof(data.action));
            data.type = enet::ActionPacketData::Type::RaidonAnim;
            enet::ToS_Action packet(data);
            mgr->mNetClient->sendPacket(&packet);
        }
        mLastRaidonAction = raidonAction;
    }

    if (mgr->mSequence->mStateSingleMode->mStageIndex == 0) // SingleModeOceanPhase0Stage (here bowser isn't the same)
        return;

    DisasterModeController* controller = DisasterModeController::tryGetController(mgr->mPlayerActor);
    if (controller == nullptr)
        return;
    SuperBowser* bowser = controller->mSuperBowser;

    const al::Nerve* curNerve = controller->getNerveKeeper()->getCurrentNerve();
    bool isNewNerve = curNerve != mLastDisasterModeControllerNrv;

    if (mIsHostClient && (mFrameCount % 120 == 0 or isNewNerve)) {
        enet::DisasterModeControllerInternalUpdateData data;
        data.mFramesOfDisaster = controller->mFramesOfDisaster;
        data.mFramesOfDisasterTransition = controller->mFramesOfDisasterTransition;
        data.mFramesOfAnticipation = controller->mFramesOfAnticipation;
        data.mFramesOfAnticipationTransition = controller->mFramesOfAnticipationTransition;
        data.mFramesOfProsperity = controller->mFramesOfProsperity;
        data.mFramesOfProsperityTransition = controller->mFramesOfProsperityTransition;
        data.mProgressFrames = controller->mProgressFrames;
        data.mFramesUntilDisaster = controller->mFramesUntilDisaster;
        data.mIsRaining = controller->mIsRaining;
        data.mPostBossPeaceTime = controller->mPostBossPeaceTime;
        data.isNewNerve = isNewNerve;
        data.controllerNerve = uintptr_t(curNerve) - hk::ro::getMainModule()->range().start();

        enet::ToS_DisasterModeControllerInternalUpdate packet(data);
        mgr->mNetClient->sendPacket(&packet);
    }
    mLastDisasterModeControllerNrv = curNerve;
}

void PlayerData::handlePosePacket(enet::ToC_PuppetPose& packet)
{
    if (!MultiplayerManager::isInSingleModeScene())
        return;
    mLastTrans = mPoseData.trans;
    mLastQuat = mPoseData.quat;
    mLastAnimRate = mPoseData.animRate;
    std::memcpy(mLastBlendWeights, mPoseData.blendWeights, sizeof(mPoseData.blendWeights));
    mPoseData = packet.getData();

    s64 interval = nn::os::GetSystemTick().m_tick - mLastPoseUpdateTimestamp;

    // dormant interval checker thing (not the best, but it should work fine)
    if (interval > (nn::os::GetSystemTickFrequency() / 1000.f) * 250)
        interval = (nn::os::GetSystemTickFrequency() / 1000.f) * 120;

    mLastPoseUpdateTimestamp = nn::os::GetSystemTick().m_tick;
    mPoseUpdateIntervalTicks += interval;
    mPoseUpdateIntervalTicks /= 2;
}

void PlayerData::handleActionPacket(enet::ToC_Action& packet)
{
    if (!MultiplayerManager::isInSingleModeScene())
        return;
    mActionQueue.pushBack(new StartActionQueueEntry { packet.getData(), MultiplayerManager::sPosePacketInterval });
}

void PlayerData::handleFigureChangePacket(enet::ToC_PlayerFigureChange& packet)
{
    EPlayerFigure figure = (EPlayerFigure)packet.getData().playerFigure;
    mFigure = figure;

    if (mPlayerPuppet == nullptr)
        return;
    if (!MultiplayerManager::isInSingleModeScene())
        return;

    mPlayerPuppet->mModelHolder->getCurrentModel()->kill();
    mPlayerPuppet->change(figure);
    mLastFigure = figure;
}

void resetRaidonSurfing()
{
    sIsSurfingRaidon = false;
}

} // namespace pe
