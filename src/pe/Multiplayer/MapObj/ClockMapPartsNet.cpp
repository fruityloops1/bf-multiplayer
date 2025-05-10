#include "pe/Multiplayer/MapObj/ClockMapPartsNet.h"
#include "al/AreaObj/AreaUtil.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "al/LiveActor/ActorMovementFunction.h"
#include "al/LiveActor/ActorPoseKeeperBase.h"
#include "al/LiveActor/ActorSceneFunction.h"
#include "al/MapObj/ChildStep.h"
#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveActionCtrl.h"
#include "al/Nerve/NerveFunction.h"
#include "al/Placement/PlacementFunction.h"
#include "al/Se/SeFunction.h"
#include "al/StageSwitch/StageSwitchKeeper.h"
#include "pe/Enet/NetClient.h"
#include <math.h>

namespace pe {

NERVE_DEF_END(ClockMapPartsNet, Rotate, RotateEnd)
NERVE_DEF(ClockMapPartsNet, RotateSign)
NERVE_DEF(ClockMapPartsNet, Wait)

ClockMapPartsNet::ClockMapPartsNet(const char* name)
    : LiveActor(name)
{
}

int ClockMapPartsNet::getTotalCycleTime() const
{
    return mWaitTime + mRotateTime + mRotateSignTime;
}

const al::Nerve* ClockMapPartsNet::getCurrentNerveByClock() const
{
    u64 clock = enet::getSyncClockFrames();
    clock += mDelayTime;
    int in = fmodf(clock, getTotalCycleTime());

    if (in <= mWaitTime)
        return &nrvClockMapPartsNetWait;
    else if (in <= mWaitTime + mRotateSignTime)
        return &nrvClockMapPartsNetRotateSign;
    else
        return &nrvClockMapPartsNetRotate;
}

int ClockMapPartsNet::getProgressIntoNrvByClock() const
{
    u64 clock = enet::getSyncClockFrames();
    clock += mDelayTime;
    int in = fmodf(clock, getTotalCycleTime());

    if (in <= mWaitTime)
        return mWaitTime;
    else if (in <= mWaitTime + mRotateSignTime)
        return in - mWaitTime;
    else
        return in - mWaitTime - mRotateSignTime;
}

void ClockMapPartsNet::init(const al::ActorInitInfo& info)
{
    al::initNerve(this, &nrvClockMapPartsNetWait);
    al::initActorPoseTQSV(this);
    al::initMapPartsActor(this, info, nullptr, al::calcChildStepCount(info));
    al::registerAreaHostMtx(this, info);
    mInitialQuat = al::getQuat(this);
    al::tryGetArg(&mClockAngle, info, "ClockAngle");
    al::tryGetArg(&mRotateAxis, info, "RotateAxis");
    al::createChildStep(info, this, true);
    al::tryGetArg(&mDelayTime, info, "DelayTime");
    al::tryGetArg(&mWaitTime, info, "WaitTime");
    al::tryGetArg(&mRotateTime, info, "RotateTime");
    if (al::isExistAction(this, "MiddleSign"))
        mRotateSignTime = al::getActionFrameMax(this, "MiddleSign");

    makeActorAppeared();
}

void ClockMapPartsNet::exeWait()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Wait");
}

void ClockMapPartsNet::exeRotate()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Rotate");

    float degrees = fmodf((float(getProgressIntoNrvByClock()) / mRotateTime) * mClockAngle + mClockAngle * int((enet::getSyncClockFrames() + mDelayTime) / getTotalCycleTime()), 360);
    al::rotateQuatLocalDirDegree(this, mInitialQuat, mRotateAxis, degrees);
}

void ClockMapPartsNet::exeRotateEnd()
{
    float degrees = fmodf(mClockAngle * int((enet::getSyncClockFrames() + mDelayTime) / getTotalCycleTime()), 360);
    al::rotateQuatLocalDirDegree(this, mInitialQuat, mRotateAxis, degrees);
    al::tryStartSe(this, "RotateEnd", nullptr);
}

void ClockMapPartsNet::exeRotateSign()
{
    if (al::isFirstStep(this))
        al::startAction(this, "MiddleSign");
}

void ClockMapPartsNet::control()
{
    const al::Nerve* nrv = getCurrentNerveByClock();
    if (!al::isNerve(this, nrv))
        al::setNerve(this, nrv);
}

} // namespace pe
