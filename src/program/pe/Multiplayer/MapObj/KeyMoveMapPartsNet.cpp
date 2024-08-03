#include "pe/Multiplayer/MapObj/KeyMoveMapPartsNet.h"
#include "al/AreaObj/AreaUtil.h"
#include "al/KeyPose/KeyPose.h"
#include "al/KeyPose/KeyPoseKeeper.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "al/LiveActor/ActorPoseKeeperBase.h"
#include "al/Placement/PlacementFunction.h"
#include "al/StageSwitch/StageSwitchKeeper.h"
#include "pe/Enet/NetClient.h"
#include "pe/Util/Log.h"

namespace pe {

KeyMoveMapPartsNet::KeyMoveMapPartsNet(const char* name)
    : LiveActor(name)
{
}

int KeyMoveMapPartsNet::calcKeyPoseFrames(int idx)
{
    const al::KeyPose* pose = mKeyPoseKeeper->getKeyPose(idx);
    const al::ByamlIter* info = pose->getPlacementInfo();
    int waitTime = 0;
    info->tryGetIntByKey(&waitTime, "WaitTime");
    int old = mKeyPoseKeeper->getCurrentIdx();
    mKeyPoseKeeper->setCurrentIdx(idx);
    int time = al::calcKeyMoveMoveTime(mKeyPoseKeeper);
    mKeyPoseKeeper->setCurrentIdx(old);
    return waitTime + time;
}

int KeyMoveMapPartsNet::calcKeyPoseFramesTotal()
{
    int sum = 0;
    for (int i = 0; i < al::getKeyPoseCount(mKeyPoseKeeper); i++)
        sum += calcKeyPoseFrames(i);
    return sum;
}

int KeyMoveMapPartsNet::calcCurKeyPoseIdxBySyncClock(int* framesIntoPose)
{
    int frames = enet::getSyncClockFrames() % calcKeyPoseFramesTotal();

    int sum = 0;
    for (int i = 0; i < al::getKeyPoseCount(mKeyPoseKeeper); i++) {
        int end = calcKeyPoseFrames(i);
        if (frames >= sum && frames < sum + end) {
            if (framesIntoPose)
                *framesIntoPose = frames - sum;
            return i;
        }
        sum += end;
    }
    return 0;
}

void KeyMoveMapPartsNet::init(const al::ActorInitInfo& info)
{
    al::initActorPoseTQSV(this);
    const char* suffixName = nullptr;
    al::tryGetStringArg(&suffixName, info, "SuffixName");
    al::initMapPartsActor(this, info, suffixName);
    mKeyPoseKeeper = al::createKeyPoseKeeper(info);
    al::registerAreaHostMtx(this, info);
    al::setKeyMoveClippingInfo(this, &mClippingVec, mKeyPoseKeeper);
    al::tryGetArg(&mDelayTime, info, "DelayTime");
    if (mDelayTime > 0)
        al::startAction(this, "Delay");
    al::tryGetArg(&mIsIgnoreFirstWaitTime, info, "IsIgnoreFirstWaitTime");
    mMtxConnector = al::tryCreateMtxConnector(this, info);
    al::trySyncStageSwitchAppearAndKill(this);
    al::tryListenStageSwitchKill(this);
}

void KeyMoveMapPartsNet::tryConnectToCollision()
{
    if (mMtxConnector)
        al::attachMtxConnectorToCollision(mMtxConnector, this, false);
}

void KeyMoveMapPartsNet::initAfterPlacement()
{
    tryConnectToCollision();
}

void KeyMoveMapPartsNet::appear()
{
    LiveActor::appear();
    tryConnectToCollision();
}

void KeyMoveMapPartsNet::kill()
{
    LiveActor::kill();
    tryConnectToCollision();
}

void KeyMoveMapPartsNet::control()
{
    int framesIntoPose = 0;
    int curKeyPoseIdx = calcCurKeyPoseIdxBySyncClock(&framesIntoPose);
    int curKeyPoseLen = calcKeyPoseFrames(curKeyPoseIdx);
    mKeyPoseKeeper->setCurrentIdx(curKeyPoseIdx);
    const al::KeyPose* pose = mKeyPoseKeeper->getCurrentKeyPose();
    int waitTime = 0;
    pose->getPlacementInfo()->tryGetIntByKey(&waitTime, "WaitTime");
    float progress = framesIntoPose <= waitTime ? 0 : float(framesIntoPose - waitTime) / (curKeyPoseLen - waitTime);

    al::calcLerpKeyTrans(al::getTransPtr(this), mKeyPoseKeeper, progress);
    al::calcSlerpKeyQuat(al::getQuatPtr(this), mKeyPoseKeeper, progress);

    if (mMtxConnector)
        al::connectPoseQT(this, mMtxConnector);
}

} // namespace pe
