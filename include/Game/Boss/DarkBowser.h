#pragma once

#include "al/LiveActor/LiveActor.h"

class DarkBowser : public al::LiveActor {
private:
    u8 _148[0x258];
    int mHealth;

public:
    void requestDamage(int amount);
};
