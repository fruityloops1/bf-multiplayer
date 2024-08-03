#pragma once

#include "al/Layout/LayoutActor.h"

class SingleModeSceneLayout;
class CounterGoalItemParts : public al::LayoutActor {
public:
    CounterGoalItemParts(const al::LayoutInitInfo& info, const char*, const char*, SingleModeSceneLayout*);

    void startDemo();
    void endDemo(bool dontCountUp);
};
