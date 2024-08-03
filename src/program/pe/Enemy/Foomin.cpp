#include "pe/Enemy/Foomin.h"
#include "Game/Player/Player/Player.h"
#include "Game/Player/PlayerActor.h"
#include "al/HitSensor/HitSensor.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorSensorFunction.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveFunction.h"
#include "al/Placement/PlacementFunction.h"

namespace pe {

namespace {
    NERVE_DEF(Foomin, Wait);
    NERVE_DEF(Foomin, Flip);
}

void Foomin::init(const al::ActorInitInfo& info)
{
    al::initActor(this, info);

    bool startWithFlip = false;
    al::tryGetArg(&startWithFlip, info, "IsStartFlip");

    if (startWithFlip)
        al::initNerve(this, &nrvFoominFlip);
    else
        al::initNerve(this, &nrvFoominWait);

    al::tryGetArg(&mMoveDir, info, "MoveDir");
    al::tryGetArg(&pMoveAmount, info, "MoveAmount");
    al::tryGetArg(&pMoveSpeed, info, "MoveSpeed");
    al::tryGetArg(&pMoveLeftRight, info, "IsMoveLeftRight");

    al::tryGetArg(&pFlingVelocityMultiplier, info, "VelocityMultiplier");
    al::tryGetArg(&pFlingYVelocityMultiplier, info, "VelocityMultiplierY");

    makeActorAppeared();
}

void Foomin::invalidateCollisionParts()
{
    al::invalidateCollisionParts(this);
    mFramesUntilCollisionPartsReactivate = 20;
}

bool Foomin::receiveMsg(const al::SensorMsg* msg, al::HitSensor* source, al::HitSensor* target)
{
    if (al::isSensorPlayer(source))
        mPlayerActor = static_cast<PlayerActor*>(al::getSensorHost(source));
    if (al::isMsgPlayerFloorTouch(msg)
        && al::isNerve(this, &nrvFoominFlip)
        && (al::getNerveStep(this) >= sFlipStartFrame && al::getNerveStep(this) <= sFlipEndFrame)) {
        invalidateCollisionParts();
        PlayerProperty& playerProperty = *static_cast<PlayerActor*>(al::getSensorHost(source))->mPlayer->mPlayerProperty;
        playerProperty.velocity = al::getFront(this) * pFlingVelocityMultiplier;
        mFramesSinceLaunch = 0;
        return true;
    }
    return false;
}

void Foomin::control()
{
    if (mFramesSinceLaunch != -1) {
        mFramesSinceLaunch++;
        if (mFramesSinceLaunch < 2 && mPlayerActor)
            mPlayerActor->mPlayer->mPlayerProperty->velocity.y = pFlingYVelocityMultiplier;
    }

    if (pMoveLeftRight) {
        const sead::Vector3f& front = al::getFront(this);
        sead::Vector3f right = { front.z, front.y, -front.x };

        if (mMoveAmount >= pMoveAmount) {
            mMoveDir = !mMoveDir;
            mMoveAmount = 0;
        }

        mMoveAmount += pMoveSpeed;
        *al::getTransPtr(this) += mMoveDir ? right * pMoveSpeed : -right * pMoveSpeed;
    }

    if (mFramesUntilCollisionPartsReactivate > 0) {
        mFramesUntilCollisionPartsReactivate--;
        if (mFramesUntilCollisionPartsReactivate == 0)
            al::validateCollisionParts(this);
    }
}

void Foomin::exeWait()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Wait");
    if (al::isActionEnd(this))
        al::setNerve(this, &nrvFoominFlip);
}

void Foomin::exeFlip()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Flip");
    if (al::isActionEnd(this))
        al::setNerve(this, &nrvFoominWait);
}

} // namespace pe