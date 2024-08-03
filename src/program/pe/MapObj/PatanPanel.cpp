#include "pe/MapObj/PatanPanel.h"
#include "al/Functor/FunctorV0M.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorClippingFunction.h"
#include "al/Math/MathQuatUtil.h"
#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveFunction.h"
#include "al/Placement/PlacementFunction.h"
#include "al/Placement/PlacementUtil.h"
#include "pe/Util/Math.h"
#include <sead/math/seadVector.h>

namespace pe {

static inline sead::Vector3f transform(const sead::Vector3f& value, const sead::Quatf& rotation)
{
    sead::Vector3f vector;
    float num12 = rotation.x + rotation.x;
    float num2 = rotation.y + rotation.y;
    float num = rotation.z + rotation.z;
    float num11 = rotation.w * num12;
    float num10 = rotation.w * num2;
    float num9 = rotation.w * num;
    float num8 = rotation.x * num12;
    float num7 = rotation.x * num2;
    float num6 = rotation.x * num;
    float num5 = rotation.y * num2;
    float num4 = rotation.y * num;
    float num3 = rotation.z * num;
    float num15 = ((value.x * ((1.0f - num5) - num3)) + (value.y * (num7 - num9))) + (value.z * (num6 + num10));
    float num14 = ((value.x * (num7 + num9)) + (value.y * ((1.0f - num8) - num3))) + (value.z * (num4 - num11));
    float num13 = ((value.x * (num6 - num10)) + (value.y * (num4 + num11))) + (value.z * ((1.0f - num8) - num5));
    vector.x = num15;
    vector.y = num14;
    vector.z = num13;
    return vector;
}

static inline float getProjectedAngleBetween(const sead::Vector3f& vecA, const sead::Vector3f& vecB)
{
    if (vecA == vecB) {
        return 0;
    }

    if (vecA == -vecB) {
        return 180;
    }

    sead::Vector3f rotationAxis;
    sead::Vector3CalcCommon<float>::cross(rotationAxis, vecA, vecB);
    rotationAxis.normalize();

    auto rotationPlaneY = vecA;
    sead::Vector3f rotationPlaneX;
    sead::Vector3CalcCommon<float>::cross(rotationPlaneX, vecA, rotationAxis);

    auto projectedVecA = sead::Vector2f(rotationPlaneX.dot(vecA), rotationPlaneY.dot(vecA));
    auto projectedVecB = sead::Vector2f(rotationPlaneX.dot(vecB), rotationPlaneY.dot(vecB));

    auto angle = std::atan2(projectedVecB.y, projectedVecB.x) - std::atan2(projectedVecA.y, projectedVecA.x);

    angle = std::fmod(angle * pe::Constants<float>::radiansToDegrees + 360, 360.0f);

    return angle;
}

void PatanPanel::rotateX(float degree, bool direction)
{
    sead::Vector3f forward = transform({ 0, 0, sPanelWidth * al::getScale(this).x / 2 }, al::getQuat(this));
    sead::Vector3f rotationOrigin = al::getTrans(this) - (direction ? forward : -forward);
    sead::Quatf right;
    al::rotateQuatRadian(&right, sead::Quatf::unit, transform(sead::Vector3f::ex, al::getQuat(this)), degree * pe::Constants<float>::degreesToRadians);

    sead::Vector3f trans = transform(al::getTrans(this) - rotationOrigin, right) + rotationOrigin;
    al::setTrans(this, trans);
    al::rotateQuatXDirDegree(al::getQuatPtr(this), al::getQuat(this), degree);
}

PatanPanel* PatanPanel::findStartPanel()
{
    PatanPanel* current = this;
    while (current->mLast != nullptr)
        current = current->mLast;
    return current;
}

namespace {
    NERVE_DEF(PatanPanel, OpenStart);
    NERVE_DEF(PatanPanel, Open);
    NERVE_DEF(PatanPanel, StartNext);
    NERVE_DEF(PatanPanel, CloseSign);
    NERVE_DEF(PatanPanel, CloseFace);
    NERVE_DEF(PatanPanel, Close);
}

void PatanPanel::init(const al::ActorInitInfo& info)
{
    al::initActor(this, info);
    al::initNerve(this, &nrvPatanPanelOpenStart);

    al::listenStageSwitchOn(this, "OpenSwitchOff", al::FunctorV0M<PatanPanel*, void (PatanPanel::*)()>(this, &PatanPanel::start));

    mInitialTrans = al::getTrans(this);
    mInitialQuat = al::getQuat(this);

    if (al::calcLinkChildNum(info, "ChildPatanPanel") > 0) {
        mNext = new PatanPanel(this);
        al::initLinksActor(mNext, info, "ChildPatanPanel", 0);

        sead::Vector3f lastUp = -transform(sead::Vector3f::ey, al::getQuat(this));
        sead::Vector3f nextUp = transform(sead::Vector3f::ey, al::getQuat(mNext));

        float degreesToRotate = getProjectedAngleBetween(lastUp, nextUp);
        if (degreesToRotate == 0)
            degreesToRotate = 180;

        mNext->mDegreesToRotate = degreesToRotate;
    }
}

void PatanPanel::exeOpenStart()
{
    if (al::isFirstStep(this))
        al::startAction(this, "OpenStart");
    if (al::getNerveStep(this) < mDegreesToRotate / 10) {
        rotateX(10, true);
    } else
        al::setNerve(this, &nrvPatanPanelOpen);
}

void PatanPanel::exeOpen()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Open");
        al::validateCollisionParts(this);
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &nrvPatanPanelStartNext);
}

void PatanPanel::exeStartNext()
{
    if (al::isFirstStep(this) && mNext)
        mNext->start();
    if (mNext == nullptr) {
        PatanPanel* first = mLast;
        while (first->mLast != nullptr)
            first = first->mLast;
        if (!first->isClosing())
            first->startClose();
    }
}

void PatanPanel::exeCloseSign()
{
    if (al::isFirstStep(this))
        al::startAction(this, "CloseSign");
    if (al::isStep(this, 25))
        al::setNerve(this, &nrvPatanPanelCloseFace);
}

void PatanPanel::exeCloseFace()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Close");
    if (al::isActionEnd(this)) {
        al::invalidateCollisionParts(this);
        al::setNerve(this, &nrvPatanPanelClose);

        if (mNext) {
            sead::Vector3f lastUp = -transform(sead::Vector3f::ey, al::getQuat(this));
            sead::Vector3f nextUp = transform(sead::Vector3f::ey, al::getQuat(mNext));

            float degreesToRotate = getProjectedAngleBetween(lastUp, nextUp);
            if (degreesToRotate == 0)
                degreesToRotate = 180;

            mDegreesToRotateToNext = degreesToRotate;
        }
    }
}

void PatanPanel::exeClose()
{
    if (mNext) {
        if (al::getNerveStep(this) < mDegreesToRotateToNext / 10)
            rotateX(10, false);
        else {
            mNext->startClose();
            al::validateClipping(this);
            makeActorDead();
        }
    } else if (al::isFirstStep(this)) {
        al::startHitReactionDisappear(this);
        al::validateClipping(this);
        al::tryOffStageSwitch(findStartPanel(), "OpenSwitchOff");
        makeActorDead();
    }
}

void PatanPanel::start()
{
    makeActorAppeared();
    al::setNerve(this, &nrvPatanPanelOpenStart);
    al::invalidateClipping(this);
    al::invalidateCollisionParts(this);

    al::setTrans(this, mInitialTrans);
    al::setQuat(this, mInitialQuat);

    if (mLast) {
        al::setTrans(this, al::getTrans(mLast));
        al::rotateQuatXDirDegree(al::getQuatPtr(this), al::getQuat(this), -mDegreesToRotate);
    } else {
        sead::Vector3f forward = transform({ 0, 0, sPanelWidth * al::getScale(this).x / 2 }, al::getQuat(this));
        *al::getTransPtr(this) -= forward * 2;
        al::rotateQuatXDirDegree(al::getQuatPtr(this), al::getQuat(this), 180);
    }
}

void PatanPanel::startClose()
{
    al::setNerve(this, &nrvPatanPanelCloseSign);
}

bool PatanPanel::isClosing() const
{
    return al::isNerve(this, &nrvPatanPanelCloseSign)
        || al::isNerve(this, &nrvPatanPanelCloseFace)
        || al::isNerve(this, &nrvPatanPanelClose);
}

} // namespace pe