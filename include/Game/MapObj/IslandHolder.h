#pragma once

#include "al/LiveActor/LiveActor.h"

class IslandHolder : public al::LiveActor {
public:
    void setScenarioID(int scenario, bool);
};
