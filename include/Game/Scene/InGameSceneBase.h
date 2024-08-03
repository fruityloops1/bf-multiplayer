#pragma once

#include "al/Scene/Scene.h"

class InGameSceneBase : public al::Scene {
public:
    virtual void prepareDestroy();
    virtual bool isRestartCheck() const;
    virtual bool isGoal() const;
    virtual bool isWorldWarp() const;
    virtual bool isTriggerPause() const;
    virtual void recordClearData() const;
};
