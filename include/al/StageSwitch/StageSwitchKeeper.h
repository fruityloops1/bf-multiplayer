#pragma once

#include "al/Functor/FunctorBase.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Util/IUseName.h"

namespace al {

class StageSwitchKeeper {
};

class IUseStageSwitch : IUseName {
    virtual StageSwitchKeeper* getStageSwitchKeeper() const = 0;
    virtual void initStageSwitchKeeper() = 0;
};

bool tryOnStageSwitch(al::IUseStageSwitch*, const char* name);
bool tryOffStageSwitch(al::IUseStageSwitch*, const char* name);
bool listenStageSwitchOn(al::IUseStageSwitch*, const char* name, const al::FunctorBase& functor);
bool listenStageSwitchOff(al::IUseStageSwitch*, const char* name, const al::FunctorBase& functor);
bool listenStageSwitchOnStart(al::IUseStageSwitch*, const al::FunctorBase& functor);
bool listenStageSwitchOffStart(al::IUseStageSwitch*, const al::FunctorBase& functor);

bool tryListenStageSwitchAppear(LiveActor* actor);
bool tryListenStageSwitchKill(LiveActor* actor);

} // namespace al