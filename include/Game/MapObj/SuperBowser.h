#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/Npc/WipeSimple.h"

class SuperBowser : public al::LiveActor {
    u8 _148[0x4a8];

public:
    al::WipeSimple* mWipeWhite = nullptr;
};
