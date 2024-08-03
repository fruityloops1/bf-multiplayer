#pragma once

#include "Game/Layout/WindowMessage.h"
#include "al/Scene/ISceneObj.h"

class GuideGameWindow : public al::ISceneObj {
    void* unk;

public:
    WindowMessage* mWindow = nullptr;

    GuideGameWindow(const al::LayoutInitInfo& info, bool something);
    const char* getSceneObjName() const override; // ガイドウィンドウ
};