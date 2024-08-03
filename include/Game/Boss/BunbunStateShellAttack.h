#pragma once

#include "al/LiveActor/ActorInitFunction.h"
#include "al/Nerve/ActorStateBase.h"

class Bunbun;
class BunbunStateShellAttack : public al::ActorStateBase {

public:
    Bunbun* mBunbun = nullptr;
    al::LiveActor* mShell = nullptr;

    BunbunStateShellAttack(Bunbun* parent, const al::ActorInitInfo& info);

    void exeChangeShell();
    void exeSpin();
};