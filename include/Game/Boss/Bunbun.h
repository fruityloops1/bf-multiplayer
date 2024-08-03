#pragma once

#include "Game/Boss/BunbunStateShellAttack.h"
#include "Game/Boss/BunbunStateSpinAttack.h"
#include "al/LiveActor/LiveActor.h"

class Bunbun : public al::LiveActor {
    void* unk;
    al::ActorStateBase* mStateStartDemo = nullptr;
    BunbunStateShellAttack* mStateShellAttack = nullptr;
    BunbunStateSpinAttack* mStateSpinAttack = nullptr;

public:
    Bunbun(const char* name);

    void control() override;
};