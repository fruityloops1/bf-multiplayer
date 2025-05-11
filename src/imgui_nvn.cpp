#include "imgui_nvn.h"
#include "agl/DisplayList.h"
#include "helpers/InputHelper.h"
#include "helpers/fsHelper.h"
#include "hk/gfx/ImGuiBackendNvn.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "imgui.h"
#include "nn/hid.h"
#include "nn/init.h"
#include "pe/DbgGui/DbgGui.h"

#include "nvn_Cpp.h"

#include "nvn/nvn_CppMethods.h"

constexpr int npad_mapping[][2] = {
    { ImGuiKey_GamepadFaceDown, static_cast<const int>(nn::hid::NpadButton::A) },
    { ImGuiKey_GamepadFaceRight, static_cast<const int>(nn::hid::NpadButton::B) },
    { ImGuiKey_GamepadFaceUp, static_cast<const int>(nn::hid::NpadButton::X) },
    { ImGuiKey_GamepadFaceLeft, static_cast<const int>(nn::hid::NpadButton::Y) },
    { ImGuiKey_GamepadL1, static_cast<const int>(nn::hid::NpadButton::L) },
    { ImGuiKey_GamepadR1, static_cast<const int>(nn::hid::NpadButton::R) },
    { ImGuiKey_GamepadL2, static_cast<const int>(nn::hid::NpadButton::ZL) },
    { ImGuiKey_GamepadR2, static_cast<const int>(nn::hid::NpadButton::ZR) },
    { ImGuiKey_GamepadStart, static_cast<const int>(nn::hid::NpadButton::Plus) },
    { ImGuiKey_GamepadBack, static_cast<const int>(nn::hid::NpadButton::Minus) },
    { ImGuiKey_GamepadDpadLeft, static_cast<const int>(nn::hid::NpadButton::Left) },
    { ImGuiKey_GamepadDpadRight, static_cast<const int>(nn::hid::NpadButton::Right) },
    { ImGuiKey_GamepadDpadUp, static_cast<const int>(nn::hid::NpadButton::Up) },
    { ImGuiKey_GamepadDpadDown, static_cast<const int>(nn::hid::NpadButton::Down) },
    { ImGuiKey_GamepadLStickLeft, static_cast<const int>(nn::hid::NpadButton::StickLLeft) },
    { ImGuiKey_GamepadLStickRight, static_cast<const int>(nn::hid::NpadButton::StickLRight) },
    { ImGuiKey_GamepadLStickUp, static_cast<const int>(nn::hid::NpadButton::StickLUp) },
    { ImGuiKey_GamepadLStickDown, static_cast<const int>(nn::hid::NpadButton::StickLDown) },
};

constexpr int mouse_mapping[][2] = {
    { ImGuiMouseButton_Left, static_cast<const int>(nn::hid::MouseButton::Left) },
    { ImGuiMouseButton_Right, static_cast<const int>(nn::hid::MouseButton::Right) },
    { ImGuiMouseButton_Middle, static_cast<const int>(nn::hid::MouseButton::Middle) },
};

constexpr int key_mapping[][2] = {
    { ImGuiKey_None, 0 },
    { ImGuiKey_Tab, static_cast<const int>(nn::hid::KeyboardKey::Tab) },
    { ImGuiKey_LeftArrow, static_cast<const int>(nn::hid::KeyboardKey::LeftArrow) },
    { ImGuiKey_RightArrow, static_cast<const int>(nn::hid::KeyboardKey::RightArrow) },
    { ImGuiKey_UpArrow, static_cast<const int>(nn::hid::KeyboardKey::UpArrow) },
    { ImGuiKey_DownArrow, static_cast<const int>(nn::hid::KeyboardKey::DownArrow) },
    { ImGuiKey_PageUp, static_cast<const int>(nn::hid::KeyboardKey::PageUp) },
    { ImGuiKey_PageDown, static_cast<const int>(nn::hid::KeyboardKey::PageDown) },
    { ImGuiKey_Home, static_cast<const int>(nn::hid::KeyboardKey::Home) },
    { ImGuiKey_End, static_cast<const int>(nn::hid::KeyboardKey::End) },
    { ImGuiKey_Insert, static_cast<const int>(nn::hid::KeyboardKey::Insert) },
    { ImGuiKey_Delete, static_cast<const int>(nn::hid::KeyboardKey::Delete) },
    { ImGuiKey_Backspace, static_cast<const int>(nn::hid::KeyboardKey::Backspace) },
    { ImGuiKey_Space, static_cast<const int>(nn::hid::KeyboardKey::Space) },
    { ImGuiKey_Enter, static_cast<const int>(nn::hid::KeyboardKey::Return) },
    { ImGuiKey_Escape, static_cast<const int>(nn::hid::KeyboardKey::Escape) },
    { ImGuiKey_LeftCtrl, static_cast<const int>(nn::hid::KeyboardKey::LeftControl) },
    { ImGuiKey_LeftShift, static_cast<const int>(nn::hid::KeyboardKey::LeftShift) },
    { ImGuiKey_LeftAlt, static_cast<const int>(nn::hid::KeyboardKey::LeftAlt) },
    { ImGuiKey_LeftSuper, static_cast<const int>(nn::hid::KeyboardKey::LeftGui) },
    { ImGuiKey_RightCtrl, static_cast<const int>(nn::hid::KeyboardKey::RightControl) },
    { ImGuiKey_RightShift, static_cast<const int>(nn::hid::KeyboardKey::RightShift) },
    { ImGuiKey_RightAlt, static_cast<const int>(nn::hid::KeyboardKey::RightAlt) },
    { ImGuiKey_RightSuper, static_cast<const int>(nn::hid::KeyboardKey::RightGui) },
    { ImGuiKey_Menu, static_cast<const int>(nn::hid::KeyboardKey::Application) }, // not positive about this
    { ImGuiKey_0, static_cast<const int>(nn::hid::KeyboardKey::D0) },
    { ImGuiKey_1, static_cast<const int>(nn::hid::KeyboardKey::D1) },
    { ImGuiKey_2, static_cast<const int>(nn::hid::KeyboardKey::D2) },
    { ImGuiKey_3, static_cast<const int>(nn::hid::KeyboardKey::D3) },
    { ImGuiKey_4, static_cast<const int>(nn::hid::KeyboardKey::D4) },
    { ImGuiKey_5, static_cast<const int>(nn::hid::KeyboardKey::D5) },
    { ImGuiKey_6, static_cast<const int>(nn::hid::KeyboardKey::D6) },
    { ImGuiKey_7, static_cast<const int>(nn::hid::KeyboardKey::D7) },
    { ImGuiKey_8, static_cast<const int>(nn::hid::KeyboardKey::D8) },
    { ImGuiKey_9, static_cast<const int>(nn::hid::KeyboardKey::D9) },
    { ImGuiKey_A, static_cast<const int>(nn::hid::KeyboardKey::A) },
    { ImGuiKey_B, static_cast<const int>(nn::hid::KeyboardKey::B) },
    { ImGuiKey_C, static_cast<const int>(nn::hid::KeyboardKey::C) },
    { ImGuiKey_D, static_cast<const int>(nn::hid::KeyboardKey::D) },
    { ImGuiKey_E, static_cast<const int>(nn::hid::KeyboardKey::E) },
    { ImGuiKey_F, static_cast<const int>(nn::hid::KeyboardKey::F) },
    { ImGuiKey_G, static_cast<const int>(nn::hid::KeyboardKey::G) },
    { ImGuiKey_H, static_cast<const int>(nn::hid::KeyboardKey::H) },
    { ImGuiKey_I, static_cast<const int>(nn::hid::KeyboardKey::I) },
    { ImGuiKey_J, static_cast<const int>(nn::hid::KeyboardKey::J) },
    { ImGuiKey_K, static_cast<const int>(nn::hid::KeyboardKey::K) },
    { ImGuiKey_L, static_cast<const int>(nn::hid::KeyboardKey::L) },
    { ImGuiKey_M, static_cast<const int>(nn::hid::KeyboardKey::M) },
    { ImGuiKey_N, static_cast<const int>(nn::hid::KeyboardKey::N) },
    { ImGuiKey_O, static_cast<const int>(nn::hid::KeyboardKey::O) },
    { ImGuiKey_P, static_cast<const int>(nn::hid::KeyboardKey::P) },
    { ImGuiKey_Q, static_cast<const int>(nn::hid::KeyboardKey::Q) },
    { ImGuiKey_R, static_cast<const int>(nn::hid::KeyboardKey::R) },
    { ImGuiKey_S, static_cast<const int>(nn::hid::KeyboardKey::S) },
    { ImGuiKey_T, static_cast<const int>(nn::hid::KeyboardKey::T) },
    { ImGuiKey_U, static_cast<const int>(nn::hid::KeyboardKey::U) },
    { ImGuiKey_V, static_cast<const int>(nn::hid::KeyboardKey::V) },
    { ImGuiKey_W, static_cast<const int>(nn::hid::KeyboardKey::W) },
    { ImGuiKey_X, static_cast<const int>(nn::hid::KeyboardKey::X) },
    { ImGuiKey_Y, static_cast<const int>(nn::hid::KeyboardKey::Y) },
    { ImGuiKey_Z, static_cast<const int>(nn::hid::KeyboardKey::Z) },
    { ImGuiKey_F1, static_cast<const int>(nn::hid::KeyboardKey::F1) },
    { ImGuiKey_F2, static_cast<const int>(nn::hid::KeyboardKey::F2) },
    { ImGuiKey_F3, static_cast<const int>(nn::hid::KeyboardKey::F3) },
    { ImGuiKey_F4, static_cast<const int>(nn::hid::KeyboardKey::F4) },
    { ImGuiKey_F5, static_cast<const int>(nn::hid::KeyboardKey::F5) },
    { ImGuiKey_F6, static_cast<const int>(nn::hid::KeyboardKey::F6) },
    { ImGuiKey_F7, static_cast<const int>(nn::hid::KeyboardKey::F7) },
    { ImGuiKey_F8, static_cast<const int>(nn::hid::KeyboardKey::F8) },
    { ImGuiKey_F9, static_cast<const int>(nn::hid::KeyboardKey::F9) },
    { ImGuiKey_F10, static_cast<const int>(nn::hid::KeyboardKey::F10) },
    { ImGuiKey_F11, static_cast<const int>(nn::hid::KeyboardKey::F11) },
    { ImGuiKey_F12, static_cast<const int>(nn::hid::KeyboardKey::F12) },
    { ImGuiKey_Apostrophe, static_cast<const int>(nn::hid::KeyboardKey::Quote) },
    { ImGuiKey_Comma, static_cast<const int>(nn::hid::KeyboardKey::Comma) },
    { ImGuiKey_Minus, static_cast<const int>(nn::hid::KeyboardKey::Minus) },
    { ImGuiKey_Period, static_cast<const int>(nn::hid::KeyboardKey::Period) },
    { ImGuiKey_Slash, static_cast<const int>(nn::hid::KeyboardKey::Slash) },
    { ImGuiKey_Semicolon, static_cast<const int>(nn::hid::KeyboardKey::Semicolon) },
    { ImGuiKey_Equal, static_cast<const int>(nn::hid::KeyboardKey::Plus) },
    { ImGuiKey_LeftBracket, static_cast<const int>(nn::hid::KeyboardKey::OpenBracket) },
    { ImGuiKey_Backslash, static_cast<const int>(nn::hid::KeyboardKey::Backslash) },
    { ImGuiKey_RightBracket, static_cast<const int>(nn::hid::KeyboardKey::CloseBracket) },
    { ImGuiKey_GraveAccent, static_cast<const int>(nn::hid::KeyboardKey::Backquote) },
    { ImGuiKey_CapsLock, static_cast<const int>(nn::hid::KeyboardKey::CapsLock) },
    { ImGuiKey_ScrollLock, static_cast<const int>(nn::hid::KeyboardKey::ScrollLock) },
    { ImGuiKey_NumLock, static_cast<const int>(nn::hid::KeyboardKey::NumLock) },
    { ImGuiKey_PrintScreen, static_cast<const int>(nn::hid::KeyboardKey::PrintScreen) },
    { ImGuiKey_Pause, static_cast<const int>(nn::hid::KeyboardKey::Pause) },
    { ImGuiKey_Keypad0, static_cast<const int>(nn::hid::KeyboardKey::NumPad0) },
    { ImGuiKey_Keypad1, static_cast<const int>(nn::hid::KeyboardKey::NumPad1) },
    { ImGuiKey_Keypad2, static_cast<const int>(nn::hid::KeyboardKey::NumPad2) },
    { ImGuiKey_Keypad3, static_cast<const int>(nn::hid::KeyboardKey::NumPad3) },
    { ImGuiKey_Keypad4, static_cast<const int>(nn::hid::KeyboardKey::NumPad4) },
    { ImGuiKey_Keypad5, static_cast<const int>(nn::hid::KeyboardKey::NumPad5) },
    { ImGuiKey_Keypad6, static_cast<const int>(nn::hid::KeyboardKey::NumPad6) },
    { ImGuiKey_Keypad7, static_cast<const int>(nn::hid::KeyboardKey::NumPad7) },
    { ImGuiKey_Keypad8, static_cast<const int>(nn::hid::KeyboardKey::NumPad8) },
    { ImGuiKey_Keypad9, static_cast<const int>(nn::hid::KeyboardKey::NumPad9) },
    { ImGuiKey_KeypadDecimal, static_cast<const int>(nn::hid::KeyboardKey::NumPadDot) },
    { ImGuiKey_KeypadDivide, static_cast<const int>(nn::hid::KeyboardKey::NumPadDivide) },
    { ImGuiKey_KeypadMultiply, static_cast<const int>(nn::hid::KeyboardKey::NumPadMultiply) },
    { ImGuiKey_KeypadSubtract, static_cast<const int>(nn::hid::KeyboardKey::NumPadSubtract) },
    { ImGuiKey_KeypadAdd, static_cast<const int>(nn::hid::KeyboardKey::NumPadAdd) },
    { ImGuiKey_KeypadEnter, static_cast<const int>(nn::hid::KeyboardKey::NumPadEnter) },
    { ImGuiKey_KeypadEqual, static_cast<const int>(nn::hid::KeyboardKey::NumPadEquals) },
};

bool hasInitImGui = false;

namespace nvnImGui {
    ImVector<ProcDrawFunc> drawQueue;
}

void disableButtons(nn::hid::NpadBaseState* state) {
    if (!InputHelper::isReadInputs() && InputHelper::isInputToggled()) {
        // clear out the data within the state (except for the sampling number and attributes)
        state->buttons = nn::hid::NpadButtonSet();
        state->analogStickL = nn::hid::AnalogStickState();
        state->analogStickR = nn::hid::AnalogStickState();
    }
}

HkTrampoline<int, int*, nn::hid::NpadFullKeyState*, int, const u32&> disableFullKeyState = hk::hook::trampoline([](int* unkInt, nn::hid::NpadFullKeyState* state, int count, const u32& port) -> int {
    int result = disableFullKeyState.orig(unkInt, state, count, port);
    disableButtons(state);
    return result;
});

HkTrampoline<int, int*, nn::hid::NpadHandheldState*, int, const u32&> disableHandheldState = hk::hook::trampoline([](int* unkInt, nn::hid::NpadHandheldState* state, int count, const u32& port) -> int {
    int result = disableHandheldState.orig(unkInt, state, count, port);
    disableButtons(state);
    return result;
});

HkTrampoline<int, int*, nn::hid::NpadJoyDualState*, int, const u32&> disableJoyDualState = hk::hook::trampoline([](int* unkInt, nn::hid::NpadJoyDualState* state, int count, const u32& port) -> int {
    int result = disableJoyDualState.orig(unkInt, state, count, port);
    disableButtons(state);
    return result;
});

HkTrampoline<int, int*, nn::hid::NpadJoyLeftState*, int, const u32&> disableJoyLeftState = hk::hook::trampoline([](int* unkInt, nn::hid::NpadJoyLeftState* state, int count, const u32& port) -> int {
    int result = disableJoyLeftState.orig(unkInt, state, count, port);
    disableButtons(state);
    return result;
});

HkTrampoline<int, int*, nn::hid::NpadJoyRightState*, int, const u32&> disableJoyRightState = hk::hook::trampoline([](int* unkInt, nn::hid::NpadJoyRightState* state, int count, const u32& port) -> int {
    int result = disableJoyRightState.orig(unkInt, state, count, port);
    disableButtons(state);
    return result;
});

void nvnImGui::addDrawFunc(ProcDrawFunc func) {
    HK_ABORT_UNLESS(!drawQueue.contains(func), "Function has already been added to queue!", 0);

    drawQueue.push_back(func);
}

bool nvnImGui::sDisableRender = true;

static void updateMouse(ImGuiIO& io) {
    ImVec2 mousePos(0, 0);
    InputHelper::getMouseCoords(&mousePos.x, &mousePos.y);
    mousePos.x *= 1.5;
    mousePos.y *= 1.5;
    io.AddMousePosEvent(mousePos.x, mousePos.y);

    ImVec2 scrollDelta(0, 0);
    InputHelper::getScrollDelta(&scrollDelta.x, &scrollDelta.y);

    if (scrollDelta.x != 0.0f)
        io.AddMouseWheelEvent(0.0f, scrollDelta.x > 0.0f ? 0.5f : -0.5f);

    for (auto [im_k, nx_k] : mouse_mapping) {
        if (InputHelper::isMousePress((nn::hid::MouseButton)nx_k))
            io.AddMouseButtonEvent((ImGuiMouseButton)im_k, true);
        else if (InputHelper::isMouseRelease((nn::hid::MouseButton)nx_k))
            io.AddMouseButtonEvent((ImGuiMouseButton)im_k, false);
    }
}

static void updateKeyboard(ImGuiIO& io) {
    for (auto [im_k, nx_k] : key_mapping) {
        if (InputHelper::isKeyPress((nn::hid::KeyboardKey)nx_k)) {
            io.AddKeyEvent((ImGuiKey)im_k, true);
        } else if (InputHelper::isKeyRelease((nn::hid::KeyboardKey)nx_k)) {
            io.AddKeyEvent((ImGuiKey)im_k, false);
        }
    }
}

static void updateGamepad(ImGuiIO& io) {
    for (auto [im_k, nx_k] : npad_mapping) {
        if (InputHelper::isButtonPress((nn::hid::NpadButton)nx_k))
            io.AddKeyEvent((ImGuiKey)im_k, true);
        else if (InputHelper::isButtonRelease((nn::hid::NpadButton)nx_k))
            io.AddKeyEvent((ImGuiKey)im_k, false);
    }
}

void nvnImGui::UpdateInput() {
    InputHelper::updatePadState();

    ImGuiIO& io = ImGui::GetIO();
    updateKeyboard(io);
    updateMouse(io);

    if (InputHelper::isInputToggled()) {
        updateGamepad(io);
    }
}

static u32 procDraw(agl::DisplayList* thisPtr) {
    u32 ret;
    defer { ret = thisPtr->endDisplayList(); };

    if (InputHelper::isButtonHold(nn::hid::NpadButton::ZR) && InputHelper::isButtonPress(nn::hid::NpadButton::Plus))
        nvnImGui::sDisableRender = !nvnImGui::sDisableRender;

    if (nvnImGui::sDisableRender)
        return ret;
    ImGui::NewFrame();

    for (auto drawFunc : nvnImGui::drawQueue) {
        drawFunc();
    }

    ImGui::Render();
    hk::gfx::ImGuiBackendNvn::instance()->draw(ImGui::GetDrawData(), thisPtr->mCmdBuf);

    return ret;
}

void nvnImGui::InstallHooks() {
    disableFullKeyState.installAtSym<"_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadFullKeyStateEiRKj">();
    disableHandheldState.installAtSym<"_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadHandheldStateEiRKj">();
    disableJoyDualState.installAtSym<"_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyDualStateEiRKj">();
    disableJoyLeftState.installAtSym<"_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyLeftStateEiRKj">();
    disableJoyRightState.installAtSym<"_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadJoyRightStateEiRKj">();

    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0096bc98, procDraw); // agl::DisplayList::endDisplayList
    hk::gfx::ImGuiBackendNvn::instance()->installHooks(false);
}

void nvnImGui::InitImGui() {
    auto* imgui = hk::gfx::ImGuiBackendNvn::instance();

    // slop
    imgui->setAllocator(
        { [](size_t allocSize, size_t alignment) -> void* {
             size_t totalSize = allocSize + sizeof(size_t) + alignment;

             void* rawBlock = nn::init::GetAllocator()->Allocate(totalSize);
             if (rawBlock == nullptr)
                 return nullptr;

             uintptr_t baseAddr = reinterpret_cast<uintptr_t>(rawBlock) + sizeof(size_t);
             uintptr_t alignedAddr = (baseAddr + alignment - 1) & ~(alignment - 1);

             size_t* metadata = reinterpret_cast<size_t*>(alignedAddr - sizeof(size_t));
             *metadata = reinterpret_cast<size_t>(rawBlock);

             return reinterpret_cast<void*>(alignedAddr);
         },

            [](void* ptr) -> void {
                if (!ptr)
                    return;

                size_t* metadata = reinterpret_cast<size_t*>(
                    reinterpret_cast<uintptr_t>(ptr) - sizeof(size_t));
                void* originalPtr = reinterpret_cast<void*>(*metadata);

                nn::init::GetAllocator()->Free(originalPtr);
            } });

    imgui->tryInitialize();

    ImGuiIO& io = ImGui::GetIO();

    InputHelper::initKBM();

    InputHelper::setPort(0); // set input helpers default port to zero

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    FsHelper::LoadData loadData = {
        .path = "sd:/BowsersFuryOnline/ImGuiData/Fonts/SFMonoSquare-Regular.otf"
    };

    FsHelper::loadFileFromPath(loadData);

    auto* font = io.Fonts->AddFontFromMemoryTTF(loadData.buffer, loadData.bufSize, 17, nullptr, io.Fonts->GetGlyphRangesJapanese());

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    io.MouseDrawCursor = true;

    ImGui::StyleColorsDark();

    style.FrameBorderSize = 0;
    style.WindowBorderSize = 0;
    style.WindowRounding = 5;
    style.ScrollbarRounding = 1;
    style.TabRounding = 4;
    style.PopupRounding = 3;
    style.FrameRounding = 3;
    style.ChildRounding = 3;

    colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.97f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.14f, 0.14f, 0.67f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.39f, 0.20f, 0.20f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0, 0, 0, 0);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.56f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.19f, 0.19f, 0.40f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(1.00f, 0.19f, 0.19f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.17f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.33f, 0.35f, 0.36f, 0.53f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.76f, 0.28f, 0.44f, 0.67f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.47f, 0.47f, 0.47f, 0.67f);
    colors[ImGuiCol_Separator] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab] = ImVec4(0.07f, 0.07f, 0.07f, 0.51f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.86f, 0.23f, 0.43f, 0.67f);
    colors[ImGuiCol_TabActive] = ImVec4(0.19f, 0.19f, 0.19f, 0.57f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.05f, 0.05f, 0.05f, 0.90f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.13f, 0.13f, 0.74f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.47f, 0.47f, 0.47f, 0.47f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1, 0, 0.00f, 0.32);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1, 0, 0.00f, 0.7);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    io.FontDefault = font;

    imgui->initTexture(true);
}