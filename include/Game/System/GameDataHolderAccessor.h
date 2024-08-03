#pragma once

#include "Game/System/GameDataHolder.h"
#include "al/Scene/SceneObjHolder.h"

class GameDataHolderWriter {
public:
    GameDataHolder* mHolder = nullptr;
};

class GameDataHolderAccessor : public GameDataHolderWriter {

public:
    GameDataHolderAccessor(const al::IUseSceneObjHolder* user);
    GameDataHolderAccessor(const al::SceneObjHolder* holder);
};
