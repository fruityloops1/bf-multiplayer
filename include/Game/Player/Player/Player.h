#pragma once

#include "Game/Player/Player/PlayerFigureDirector.h"
#include "Game/Player/PlayerActionGraph.h"
#include <sead/math/seadVector.h>

struct PlayerProperty {
    sead::Vector3f trans;
    u8 unk[0x18];
    sead::Vector3f velocity;
};

class Player {
public:
    PlayerProperty* mPlayerProperty = nullptr;
    u8 unk[0x80];
    PlayerFigureDirector* mFigureDirector = nullptr;
    void* _90;
    void* _98;
    PlayerActionGraph* mActionGraph = nullptr;
};