#pragma once

#include "Game/Player/PlayerRetargettingSelector.h"
#include "al/Scene/ISceneObj.h"

class PlayerRetargettingSelectorSceneObj : public PlayerRetargettingSelector, public al::ISceneObj {
public:
    const char* getSceneObjName() const override;
};