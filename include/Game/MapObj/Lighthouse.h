#pragma once

#include "al/LiveActor/LiveActor.h"

class Lighthouse : public al::LiveActor {
public:
    Lighthouse(const char* name);

    void killInkPillar();
    void setScenarioAnim();
    void setInkMeNot(bool);
    void setScenarioButtons(al::LiveActor* actor);
    void setCurrentScenarioID(int scenarioID);

    u8 unk[0x68];
    int mNextScenarioID;
};