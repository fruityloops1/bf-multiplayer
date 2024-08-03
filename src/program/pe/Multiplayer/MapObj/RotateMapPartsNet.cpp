#include "pe/Multiplayer/MapObj/RotateMapPartsNet.h"
#include "al/Functor/FunctorV0M.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "al/LiveActor/ActorMovementFunction.h"
#include "al/LiveActor/ActorPoseKeeperBase.h"
#include "al/MapObj/ChildStep.h"
#include "al/Nerve/NerveFunction.h"
#include "al/Placement/PlacementFunction.h"
#include "al/Se/SeFunction.h"
#include "al/StageSwitch/StageSwitchKeeper.h"
#include "pe/Enet/NetClient.h"

namespace pe {

NERVE_DEF(RotateMapPartsNet, StandBy)
NERVE_DEF(RotateMapPartsNet, Rotate)

RotateMapPartsNet::RotateMapPartsNet(const char* name)
    : LiveActor(name)
{
}

void RotateMapPartsNet::init(const al::ActorInitInfo& info)
{
    al::initNerve(this, &nrvRotateMapPartsNetRotate);
    al::initActorPoseTQSV(this);
    al::initMapPartsActor(this, info, nullptr, al::calcChildStepCount(info));
    al::tryGetArg(&mRotateAxis, info, "RotateAxis");
    al::tryGetArg(&mRotateSpeed, info, "RotateSpeed");
    al::createChildStep(info, this, true);

    mInitialQuat = al::getQuat(this);
    if (al::tryGetArg(&mIsTriggerEffectOnAngle, info, "IsTriggerEffectOnAngle"))
        al::tryGetArg(&mEffectTriggerAngle, info, "EffectTriggerAngle");

    if (al::listenStageSwitchOnStart(this, al::FunctorV0M(this, &RotateMapPartsNet::start)))
        al::setNerve(this, &nrvRotateMapPartsNetStandBy);

    al::trySyncStageSwitchAppear(this);
}

void RotateMapPartsNet::start()
{
    if (!al::isNerve(this, &nrvRotateMapPartsNetRotate))
        al::setNerve(this, &nrvRotateMapPartsNetRotate);
}

void RotateMapPartsNet::exeStandBy() { }

void RotateMapPartsNet::exeRotate()
{
    float degrees = fmodf(enet::getSyncClockFrames() * (mRotateSpeed / 100.f), 360);
    al::rotateQuatLocalDirDegree(this, mInitialQuat, mRotateAxis, degrees);

    if (mIsTriggerEffectOnAngle && degrees == mEffectTriggerAngle) {
        al::tryStartEffectAction(this, "EffectOnAngle");
    }

    if (al::isExistSePlayNameInUserInfo(this, "RotateWithSpeed"))
        al::tryHoldSeWithParam(this, "RotateWithSpeed", mRotateSpeed, nullptr);
}

} // namespace pe
