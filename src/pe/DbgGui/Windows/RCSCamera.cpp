#include "pe/DbgGui/Windows/RCSCamera.h"
#include "Game/Player/IUsePlayerRetargettingSelector.h"
#include "Game/Player/PlayerActor.h"
#include "al/Camera/CameraDirector.h"
#include "al/Camera/CameraDirector_RS.h"
#include "al/Camera/CameraPoser_RS.h"
#include "al/Camera/CameraUtil.h"
#include "al/Controller/ControllerUtil.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "al/Math/MathLerpUtil.h"
#include "al/Placement/PlacementUtil.h"
#include "gfx/seadCamera.h"
#include "helpers/InputHelper.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "imgui.h"
#include "math/seadMathCalcCommon.h"
#include "math/seadQuatCalcCommon.h"
#include "nn/hid.h"
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

#define makeLookAtCameraRCHookImpl(HOOK)                                                                                                                                          \
    HkTrampoline<void, al::CameraPoser*, sead::LookAtCamera*> makeLookAtCameraHook_##HOOK = hk::hook::trampoline([](al::CameraPoser* poser, sead::LookAtCamera* camera) -> void { \
        makeLookAtCameraHook_##HOOK.orig(poser, camera);                                                                                                                          \
        *camera = sIsEnabledCamera ? sead::LookAtCamera(sPos, sAt, sUp) : sead::LookAtCamera(sLastCamera.getPos(), sLastCamera.getAt(), sLastCamera.getUp());                     \
    });

    // clang-format off
    makeLookAtCameraRCHookImpl(CameraPoserAnim);
    makeLookAtCameraRCHookImpl(CameraPoserEntrance);
    makeLookAtCameraRCHookImpl(CameraPoserFixedAll);
    makeLookAtCameraRCHookImpl(CameraPoserFixedPoint);
    makeLookAtCameraRCHookImpl(CameraPoserKinopioBrigade);
    makeLookAtCameraRCHookImpl(CameraPoserParallel);
    makeLookAtCameraRCHookImpl(CameraPoserParallelOnRail);
    makeLookAtCameraRCHookImpl(CameraPoserProgramable);
    makeLookAtCameraRCHookImpl(CameraPoserRail);
    makeLookAtCameraRCHookImpl(CameraPoserRailTower);
    makeLookAtCameraRCHookImpl(CameraPoserTower);
    makeLookAtCameraRCHookImpl(CameraPoserZoom);
    // clang-format on

    HkTrampoline<void, al::CameraPoseUpdater*> disableSwitchHook = [](al::CameraPoseUpdater* pUpdater) {
        if (!sIsEnabledCamera)
            disableSwitchHook.orig(pUpdater);
    };

    HkTrampoline<void, al::CameraPoser_RS*, sead::LookAtCamera*> makeLookAtCameraRSHook = [](al::CameraPoser_RS* poser, sead::LookAtCamera* camera) {
        makeLookAtCameraRSHook.orig(poser, &sLastCamera);

        if (InputHelper::isKeyHold(nn::hid::KeyboardKey::N) || (al::isPadHoldZL() && al::isPadHoldZR() && al::isPadTriggerA())) {
            sAnimate = true;
            sFrameCount = 0;
        }
        if (InputHelper::isKeyHold(nn::hid::KeyboardKey::M))
            sAnimate = false;

        *camera = sIsEnabledCamera ? sead::LookAtCamera(sPos, sAt, sUp) : sead::LookAtCamera(sLastCamera.getPos(), sLastCamera.getAt(), sLastCamera.getUp());
    };

    RCSCamera::RCSCamera()
    {
        disableSwitchHook.installAtOffset(hk::ro::getMainModule(), 0x00923c90);

        makeLookAtCameraRSHook.installAtOffset(hk::ro::getMainModule(), 0x00923170);

        makeLookAtCameraHook_CameraPoserAnim.installAtOffset(hk::ro::getMainModule(), 0x00906010);
        makeLookAtCameraHook_CameraPoserEntrance.installAtOffset(hk::ro::getMainModule(), 0x00906500);
        makeLookAtCameraHook_CameraPoserFixedAll.installAtOffset(hk::ro::getMainModule(), 0x00906a80);
        makeLookAtCameraHook_CameraPoserFixedPoint.installAtOffset(hk::ro::getMainModule(), 0x00907060);
        makeLookAtCameraHook_CameraPoserKinopioBrigade.installAtOffset(hk::ro::getMainModule(), 0x00907960);
        makeLookAtCameraHook_CameraPoserParallel.installAtOffset(hk::ro::getMainModule(), 0x0090cc10);
        makeLookAtCameraHook_CameraPoserParallelOnRail.installAtOffset(hk::ro::getMainModule(), 0x0090e510);
        makeLookAtCameraHook_CameraPoserProgramable.installAtOffset(hk::ro::getMainModule(), 0x0090ec00);
        makeLookAtCameraHook_CameraPoserRail.installAtOffset(hk::ro::getMainModule(), 0x0090f180);
        makeLookAtCameraHook_CameraPoserRailTower.installAtOffset(hk::ro::getMainModule(), 0x0090f9a0);
        makeLookAtCameraHook_CameraPoserTower.installAtOffset(hk::ro::getMainModule(), 0x00910580);
        makeLookAtCameraHook_CameraPoserZoom.installAtOffset(hk::ro::getMainModule(), 0x00912130);
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
