#pragma once

#include "Game/Npc/NekoNormal.h"
#include "al/LiveActor/LiveActor.h"

class NekoParent : public al::LiveActor {
public:
    void tryCollectNeko(IUseNekoModeActor* neko);
};
