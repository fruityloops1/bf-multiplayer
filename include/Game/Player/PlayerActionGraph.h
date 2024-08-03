#pragma once

#include "Game/Player/PlayerActionNode.h"

class PlayerActionGraph {
private:
    PlayerActionNode* mCurrentNode;
    void* _8;
    void* _10;
    bool _18;

public:
    PlayerActionNode* getCurrentNode() const { return mCurrentNode; }

    void forceChange(PlayerActionNode* node);
    void setRoot(PlayerActionNode* node);
    void init();
    void move();
    void update();
    void updateCurrentAction();
    void checkShift();
    void checkShiftCondition();

    PlayerAction* getAction() const;
    PlayerAction* getActionNonConst();
};
