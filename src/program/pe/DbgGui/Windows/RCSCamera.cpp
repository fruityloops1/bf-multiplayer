#include "pe/DbgGui/Windows/RCSCamera.h"
#include "Game/Player/IUsePlayerRetargettingSelector.h"
#include "Game/Player/PlayerActor.h"
#include "al/Camera/CameraDirector_RS.h"
#include "al/Camera/CameraPoser_RS.h"
#include "al/Camera/CameraUtil.h"
#include "al/Controller/ControllerUtil.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "al/Math/MathLerpUtil.h"
#include "al/Placement/PlacementUtil.h"
#include "gfx/seadCamera.h"
#include "helpers/InputHelper.h"
#include "hook/trampoline.hpp"
#include "imgui.h"
#include "math/seadMathCalcCommon.h"
#include "math/seadQuatCalcCommon.h"
#include "nn/hid.h"
#include "patch/code_patcher.hpp"
#include "pe/Util/Log.h"
#include "pe/Util/Math.h"

namespace pe {
namespace gui {

    static bool sIsEnabledCamera = false;
    static sead::Vector3f sPos(sead::Vector3f::zero), sAt(sead::Vector3f::zero), sUp(sead::Vector3f::ey);
    static sead::LookAtCamera sLastCamera;
    static sead::Vector2f sLastMousePos(sead::Vector2f::zero);
    static sead::Vector2f sMouseDelta(sead::Vector2f::zero);
    static float sCameraSpeed = 2;

    static sead::Vector3f sPosLerpFrom(sead::Vector3f::zero), sFrontLerpFrom(sead::Vector3f::zero), sUpLerpFrom(sead::Vector3f::ey);
    static sead::Vector3f sPosLerpTo(sead::Vector3f::zero), sFrontLerpTo(sead::Vector3f::zero), sUpLerpTo(sead::Vector3f::ey);

    static bool sAnimate = false;
    static int sAnimateFrames = 60;
    static s64 sFrameCount = 0;
    static bool sEaseInOut = false;

    static float calcAnimatePos()
    {
        return sead::Mathf::clamp(float(sFrameCount) / sAnimateFrames, 0, 1);
    }

    HOOK_DEFINE_TRAMPOLINE(DisableSwitch) { static void Callback(al::CameraPoseUpdater*); };

    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraRSHook) { static void Callback(al::CameraPoser_RS * poser, sead::LookAtCamera * camera); };

#define makeLookAtCameraRCHookImpl() \
    Orig(poser, &sLastCamera);       \
    *camera = sIsEnabledCamera ? sead::LookAtCamera(sPos, sAt, sUp) : sead::LookAtCamera(sLastCamera.getPos(), sLastCamera.getAt(), sLastCamera.getUp());

    // clang-format off
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserAnim) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserEntrance) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserFixedAll) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserFixedPoint) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserKinopioBrigade) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserParallel) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserParallelOnRail) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserProgramable) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserRail) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserRailTower) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserTower) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
    HOOK_DEFINE_TRAMPOLINE(MakeLookAtCameraHook_CameraPoserZoom) { static void Callback(al::CameraPoser * poser, sead::LookAtCamera * camera) { makeLookAtCameraRCHookImpl(); } };
// clang-format on

void DisableSwitch::Callback(al::CameraPoseUpdater* pUpdater)
{
    if (!sIsEnabledCamera)
        Orig(pUpdater);
}

void MakeLookAtCameraRSHook::Callback(al::CameraPoser_RS* poser, sead::LookAtCamera* camera)
{
    Orig(poser, &sLastCamera);

    if (InputHelper::isKeyHold(nn::hid::KeyboardKey::N) || (al::isPadHoldZL() && al::isPadHoldZR() && al::isPadTriggerA())) {
        sAnimate = true;
        sFrameCount = 0;
    }
    if (InputHelper::isKeyHold(nn::hid::KeyboardKey::M))
        sAnimate = false;

    *camera = sIsEnabledCamera ? sead::LookAtCamera(sPos, sAt, sUp) : sead::LookAtCamera(sLastCamera.getPos(), sLastCamera.getAt(), sLastCamera.getUp());
}

RCSCamera::RCSCamera()
{
    DisableSwitch::InstallAtOffset(0x00923c90);

    MakeLookAtCameraRSHook::InstallAtOffset(0x00923170);

    MakeLookAtCameraHook_CameraPoserAnim::InstallAtOffset(0x00906010);
    MakeLookAtCameraHook_CameraPoserEntrance::InstallAtOffset(0x00906500);
    MakeLookAtCameraHook_CameraPoserFixedAll::InstallAtOffset(0x00906a80);
    MakeLookAtCameraHook_CameraPoserFixedPoint::InstallAtOffset(0x00907060);
    MakeLookAtCameraHook_CameraPoserKinopioBrigade::InstallAtOffset(0x00907960);
    MakeLookAtCameraHook_CameraPoserParallel::InstallAtOffset(0x0090cc10);
    MakeLookAtCameraHook_CameraPoserParallelOnRail::InstallAtOffset(0x0090e510);
    MakeLookAtCameraHook_CameraPoserProgramable::InstallAtOffset(0x0090ec00);
    MakeLookAtCameraHook_CameraPoserRail::InstallAtOffset(0x0090f180);
    MakeLookAtCameraHook_CameraPoserRailTower::InstallAtOffset(0x0090f9a0);
    MakeLookAtCameraHook_CameraPoserTower::InstallAtOffset(0x00910580);
    MakeLookAtCameraHook_CameraPoserZoom::InstallAtOffset(0x00912130);
}

static sead::Vector3f rotateVectorY(const sead::Vector3f& vector, float angle)
{
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);
    return {
        vector.x * cosAngle - vector.z * sinAngle,
        vector.y,
        vector.x * sinAngle + vector.z * cosAngle
    };
}

void RCSCamera::updateCameraInput()
{
    sead::Vector2f mousePos;
    InputHelper::getMouseCoords(&mousePos.x, &mousePos.y);

    sMouseDelta = mousePos - sLastMousePos;
    sLastMousePos = mousePos;

    sead::Vector2f scrollDelta;
    InputHelper::getScrollDelta(&scrollDelta.x, &scrollDelta.y);

    if (InputHelper::isMouseHold(nn::hid::MouseButton::Middle)) {
        sCameraSpeed += scrollDelta.x / 480;

        sAt = sPos + mDirection;

        mWheelMoveVel /= 1.4;
        mCameraMoveVel /= 1.4;
        return;
    }

    mWheelMoveVel += scrollDelta.x;
    if ((sMouseDelta.x != 0 || sMouseDelta.y != 0) && InputHelper::isMouseHold(nn::hid::MouseButton::Right))
        mCameraMoveVel += sMouseDelta * .0014f;

    mDirection = rotateVectorY(mDirection, mCameraMoveVel.x);
    mDirection.y += -mCameraMoveVel.y;
    mDirection.normalize();

    if (mWheelMoveVel != 0) {
        float degree = (mousePos.x + float(1280) / 2) / 1280 - 1;
        sead::Vector3f dir = rotateVectorY(mDirection, degree);
        dir.y += -(mousePos.y / 720 - 0.5);
        sPos += dir * (mWheelMoveVel * .25) * sCameraSpeed;
    }

    if (InputHelper::isKeyHold(nn::hid::KeyboardKey::W) || InputHelper::isKeyHold(nn::hid::KeyboardKey::S)) {
        float speed = sCameraSpeed * 40;

        if (InputHelper::isKeyHold(nn::hid::KeyboardKey::W))
            sPos += mDirection * speed;
        else
            sPos -= mDirection * speed;
    }

    if (InputHelper::isKeyHold(nn::hid::KeyboardKey::A) || InputHelper::isKeyHold(nn::hid::KeyboardKey::D)) {
        float speed = sCameraSpeed * 40;

        sead::Vector3f sideVec(mDirection.z, mDirection.y, -mDirection.x);

        if (InputHelper::isKeyHold(nn::hid::KeyboardKey::A))
            sPos += sideVec * speed;
        else
            sPos -= sideVec * speed;
    }

    if (InputHelper::isKeyHold(nn::hid::KeyboardKey::Q) || InputHelper::isKeyHold(nn::hid::KeyboardKey::E)) {
        float speed = sCameraSpeed * 40;

        if (InputHelper::isKeyHold(nn::hid::KeyboardKey::Q))
            sPos += sead::Vector3f::ey * speed;
        else
            sPos -= sead::Vector3f::ey * speed;
    }

    sAt = sPos + mDirection;

    mWheelMoveVel /= 1.4;
    mCameraMoveVel /= 1.4;
}

static bool sDrawVisible = false;

void RCSCamera::update()
{
    if (sDrawVisible) {
        if (sAnimate) {
            sFrameCount++;
            float progress = calcAnimatePos();

            if (sEaseInOut)
                progress = easeInOutSine(progress);

            al::lerpVec(&sPos, sPosLerpFrom, sPosLerpTo, progress);
            sAt = sPos + slerp(sFrontLerpFrom, sFrontLerpTo, progress);
            al::lerpVec(&sUp, sUpLerpFrom, sUpLerpTo, progress);
        } else
            updateCameraInput();
    }
}

void RCSCamera::draw()
{
    sDrawVisible = true;
    if (getDbgGuiSharedData().showRCSCamera) {
        if (ImGui::Begin("RCSCamera", &getDbgGuiSharedData().showRCSCamera)) {
            if (ImGui::Checkbox("Enable", &sIsEnabledCamera)) {
                sPos = sLastCamera.getPos();
                sAt = sLastCamera.getAt();
                sUp = sLastCamera.getUp();
                mDirection = sAt - sPos;
                mDirection.normalize();
            }

            ImGui::DragFloat("Camera Control Speed", &sCameraSpeed, 0.05);

            if (sIsEnabledCamera) {
                ImGui::DragFloat3("Pos", sPos.e.data());
                ImGui::DragFloat3("At", sAt.e.data());
                ImGui::DragFloat3("Up", sUp.e.data());
            }

            if (ImGui::Button("Set PosLerpFrom")) {
                sPosLerpFrom = sPos;
            }
            if (ImGui::Button("Set AtLerpFrom")) {
                sFrontLerpFrom = sAt - sPos;
            }
            if (ImGui::Button("Set UpLerpFrom")) {
                sUpLerpFrom = sUp;
            }
            if (ImGui::Button("Set PosLerpTo")) {
                sPosLerpTo = sPos;
            }
            if (ImGui::Button("Set AtLerpTo")) {
                sFrontLerpTo = sAt - sPos;
            }
            if (ImGui::Button("Set UpLerpTo")) {
                sUpLerpTo = sUp;
            }

            ImGui::Checkbox("EaseInOut", &sEaseInOut);
            ImGui::SliderInt("AnimateLength", &sAnimateFrames, 0, 1000);
            ImGui::Text("Progress: %f", calcAnimatePos());

            if (ImGui::Button("Start Anim")) {
                sAnimate = true;
                sFrameCount = 0;
            }
            if (ImGui::Button("Stop Anim")) {
                sAnimate = false;
            }
        }
        ImGui::End();
    }
}

} // namespace gui
} // namespace pe
