#pragma once

#include "Game/Layout/ButtonGroup.h"
#include "al/Layout/LayoutActor.h"

enum class TopMenuAppearState {
    Zero = 0,
    SingleMode = 2
};

class GameDataHolder;
class StageWipeKeeper;
class TopMenuScene;
class TopMenu : public al::LayoutActor {
protected:
    GameDataHolder* mGameDataHolder = nullptr;
    StageWipeKeeper* mWipeKeeper = nullptr;
    ButtonGroup* mNewGameResumeButtonGroup = nullptr;
    TopMenuScene* mScene = nullptr;
    TopMenuAppearState mAppearState = TopMenuAppearState::Zero;
    void* _150 = nullptr;
    void* _158 = nullptr;

public:
    TopMenu(TopMenuScene* scene, const al::LayoutInitInfo& info, GameDataHolder* holder, StageWipeKeeper* wipeKeeper, const char* something);

    void appear(TopMenuAppearState state);
};
