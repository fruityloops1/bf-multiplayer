#pragma once

#include "al/LiveActor/LiveActor.h"

class Coin : public al::LiveActor {
    u8 unk[0x40];

public:
    Coin();
    Coin(const char* name);

    void kill() override;
};
