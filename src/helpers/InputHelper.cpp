#include "InputHelper.h"

static const char* styleNames[] = {
    "Pro Controller",
    "Joy-Con controller in handheld mode",
    "Joy-Con controller in dual mode",
    "Joy-Con left controller in single mode",
    "Joy-Con right controller in single mode",
    "GameCube controller",
    "Poké Ball Plus controller",
    "NES/Famicom controller",
    "NES/Famicom controller in handheld mode",
    "SNES controller",
    "N64 controller",
    "Sega Genesis controller",
    "generic external controller",
    "generic controller",
};

nn::hid::NpadBaseState InputHelper::prevControllerState {};
nn::hid::NpadBaseState InputHelper::curControllerState {};

nn::hid::KeyboardState InputHelper::curKeyboardState {};
nn::hid::KeyboardState InputHelper::prevKeyboardState {};

nn::hid::MouseState InputHelper::curMouseState {};
nn::hid::MouseState InputHelper::prevMouseState {};

ulong InputHelper::selectedPort = -1;
bool InputHelper::isReadInput = true;
bool InputHelper::toggleInput = true;

const char* getStyleName(nn::hid::NpadStyleSet style)
{

    u32 index = -1;

    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleFullKey)) {
        index = 0;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleHandheld)) {
        index = 1;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleJoyDual)) {
        index = 2;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleJoyLeft)) {
        index = 3;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleJoyRight)) {
        index = 4;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleGc)) {
        index = 5;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStylePalma)) {
        index = 6;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleLark)) {
        index = 7;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleHandheldLark)) {
        index = 8;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleLucia)) {
        index = 9;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleLagon)) {
        index = 10;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleLager)) {
        index = 11;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleSystemExt)) {
        index = 12;
    }
    if (style.IsBitSet(nn::hid::NpadStyleTag::NpadStyleSystem)) {
        index = 13;
    }

    if (index != -1) {
        return styleNames[index];
    } else {
        return "Unknown";
    }
}

void InputHelper::initKBM()
{
    nn::hid::InitializeKeyboard();
    nn::hid::InitializeMouse();
}

void InputHelper::updatePadState()
{
    prevControllerState = curControllerState;
    tryGetContState(&curControllerState, selectedPort);

    prevKeyboardState = curKeyboardState;
    nn::hid::GetKeyboardState(&curKeyboardState);

    prevMouseState = curMouseState;
    nn::hid::GetMouseState(&curMouseState);

    // if (isHoldZL() && isPressZR()) {
    //     toggleInput = !toggleInput;
    // }

    toggleInput = false;
}

bool InputHelper::tryGetContState(nn::hid::NpadBaseState* state, ulong port)
{

    nn::hid::NpadStyleSet styleSet = nn::hid::GetNpadStyleSet(port);
    isReadInput = true;
    bool result = true;

    if (styleSet.IsBitSet(nn::hid::NpadStyleTag::NpadStyleFullKey)) {
        nn::hid::GetNpadState((nn::hid::NpadFullKeyState*)state, port);
    } else if (styleSet.IsBitSet(nn::hid::NpadStyleTag::NpadStyleHandheld)) {
        nn::hid::GetNpadState((nn::hid::NpadHandheldState*)state, port);
    } else if (styleSet.IsBitSet(nn::hid::NpadStyleTag::NpadStyleJoyDual)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyDualState*)state, port);
    } else if (styleSet.IsBitSet(nn::hid::NpadStyleTag::NpadStyleJoyLeft)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyLeftState*)state, port);
    } else if (styleSet.IsBitSet(nn::hid::NpadStyleTag::NpadStyleJoyRight)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyRightState*)state, port);
    } else {
        result = false;
    }

    isReadInput = false;

    return result;
}

bool InputHelper::isButtonHold(nn::hid::NpadButton button)
{
    return curControllerState.buttons.IsBitSet(button);
}

bool InputHelper::isButtonPress(nn::hid::NpadButton button)
{
    return curControllerState.buttons.IsBitSet(button) && !prevControllerState.buttons.IsBitSet(button);
}

bool InputHelper::isButtonRelease(nn::hid::NpadButton button)
{
    return !curControllerState.buttons.IsBitSet(button) && prevControllerState.buttons.IsBitSet(button);
}

bool InputHelper::isKeyHold(nn::hid::KeyboardKey key)
{
    return curKeyboardState.keys.IsBitSet(key);
}

bool InputHelper::isKeyPress(nn::hid::KeyboardKey key)
{
    return curKeyboardState.keys.IsBitSet(key) && !prevKeyboardState.keys.IsBitSet(key);
}

bool InputHelper::isKeyRelease(nn::hid::KeyboardKey key)
{
    return !curKeyboardState.keys.IsBitSet(key) && prevKeyboardState.keys.IsBitSet(key);
}

bool InputHelper::isMouseHold(nn::hid::MouseButton button)
{
    return curMouseState.buttons.IsBitSet(button);
}

bool InputHelper::isMousePress(nn::hid::MouseButton button)
{
    return curMouseState.buttons.IsBitSet(button) && !prevMouseState.buttons.IsBitSet(button);
}

bool InputHelper::isMouseRelease(nn::hid::MouseButton button)
{
    return !curMouseState.buttons.IsBitSet(button) && prevMouseState.buttons.IsBitSet(button);
}

void InputHelper::getMouseCoords(float* x, float* y)
{
    *x = curMouseState.x;
    *y = curMouseState.y;
}

void InputHelper::getScrollDelta(float* x, float* y)
{
    *x = curMouseState.wheelDeltaX;
    *y = curMouseState.wheelDeltaY;
}