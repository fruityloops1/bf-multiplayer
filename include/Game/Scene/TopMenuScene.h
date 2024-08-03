#pragma once

#include "Game/Layout/RCSControlGuideBar.h"
#include "Game/Layout/TopMenu.h"
#include "Game/Sequence/StageWipeKeeper.h"
#include "Game/System/GameDataHolder.h"
#include "al/Scene/Scene.h"
#include <sead/gfx/seadCamera.h>

class TopMenuScene : public al::Scene {
private:
    GameDataHolder* mGameDataHolder = nullptr;
    sead::Viewport* mViewport = nullptr;
    void* _FC = nullptr;
    TopMenu* mTopMenuLayout = nullptr;
    RCSControlGuideBar* mControlGuideBar = nullptr;
    StageWipeKeeper* mWipeKeeper = nullptr;
    class WindowConfirm* _118 = nullptr;
    class RCS_SaveDataLayout* mSaveDataLayout = nullptr;
    void* _128 = nullptr;
    void* _130 = nullptr;

public:
    TopMenuScene(StageWipeKeeper* keeper);
    ~TopMenuScene() override;

    void init(const al::SceneInitInfo& info) override;

    void appear() override;
    void control() override;
    void drawMain_() const override;

    virtual void appear(TopMenuAppearState state, bool);
};
