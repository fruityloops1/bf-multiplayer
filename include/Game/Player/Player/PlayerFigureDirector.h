#pragma once

#include "Game/Player/PlayerAnimFunc.h"
#include "al/LiveActor/LiveActor.h"

class PlayerFigureDirector {
public:
    int unk = 0;
    EPlayerFigure mCurFigure = EPlayerFigure::None;

    void change(EPlayerFigure figure);
    void forceChange(EPlayerFigure figure);
    void set(EPlayerFigure figure);
    void lose();
    void update();
};

namespace rc {

void initPlayerFigureType(al::LiveActor* actor, u32 figureType, bool something);

} // namespace rc
