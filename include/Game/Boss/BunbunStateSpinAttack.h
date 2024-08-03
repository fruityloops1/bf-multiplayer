#pragma once

#include "al/LiveActor/ActorInitFunction.h"
#include "al/Nerve/ActorStateBase.h"

class Bunbun;
class BunbunStateSpinAttack : public al::ActorStateBase {

public:
    Bunbun* mBunbun = nullptr;
    al::LiveActor* mSpinArm = nullptr;

    BunbunStateSpinAttack(Bunbun* parent, const al::ActorInitInfo& info);

    void exeAttack();
    void exeAttackEnd();
    void exeAttackSign();
    void exeAttackStart();
};