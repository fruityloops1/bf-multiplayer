#pragma once

#include <sead/basis/seadTypes.h>

namespace al {

class ActionBgmCtrl;
class ActionFlagCtrl;
class ActionEffectCtrl;
class ActionScreenEffectCtrl;
class ActionSeCtrl;
class ActionOceanWaveCtrl;
class LiveActor;

class ActorActionKeeper {
private:
    LiveActor* mParent;
    u8 _8[0x18];
    void* _20;
    ActionFlagCtrl* mFlagCtrl = nullptr;
    ActionEffectCtrl* mEffectCtrl = nullptr;
    ActionSeCtrl* mSeCtrl = nullptr;
    ActionBgmCtrl* mBgmCtrl = nullptr;
    ActionOceanWaveCtrl* mOceanWaveCtrl = nullptr;
    ActionScreenEffectCtrl* mScreenEffectCtrl = nullptr;
    void* _58 = nullptr;

public:
    void startAction(const char* action);
};

} // namespace al