#pragma once

#include "al/Layout/LayoutInitInfo.h"
#include "al/Nerve/NerveActionCtrl.h"
#include <sead/math/seadMatrix.h>
#include <sead/prim/seadSafeString.h>

namespace al {

class LiveActor;
class Nerve;

struct ActorSceneInfo;
class ExecuteDirector;
class AudioDirector;
class EffectSystemInfo;
class OceanWaveDirector;
class HitSensorDirector;
class StageSwitchDirector;
class ScreenPointDirector;
class LiveActorGroup;
class PlacementId;
class PlacementInfo;

class ActorInitInfo {
    const PlacementInfo* mPlacementInfo = nullptr;
    const LayoutInitInfo* mLayoutInitInfo = nullptr;
    const ActorSceneInfo* mActorSceneInfo = nullptr;
    const ExecuteDirector* mExecuteDirector = nullptr;
    const AudioDirector* mAudioDirector = nullptr;
    const EffectSystemInfo* mEffectSystemInfo = nullptr;
    const OceanWaveDirector* mOceanWaveDirector = nullptr;
    const HitSensorDirector* mHitSensorDirector = nullptr;
    const StageSwitchDirector* mStageSwitchDirector = nullptr;
    const ScreenPointDirector* mScreenPointDirector = nullptr;
    const PlacementId* mPlacementId = nullptr;
    const LiveActorGroup* _D0 = nullptr;
    u8 _60[0x78];

public:
    ActorInitInfo() = default;

    void initNoViewId(const PlacementInfo*, const ActorInitInfo&);

    const ActorSceneInfo* getSceneInfo() const { return mActorSceneInfo; }
};

const PlacementInfo& getPlacementInfo(const ActorInitInfo& info);
const LayoutInitInfo& getLayoutInitInfo(const ActorInitInfo& info);

void initActor(LiveActor* actor, const ActorInitInfo& info);
void initMapPartsActor(LiveActor* actor, const ActorInitInfo& info, const char* subArchiveName = nullptr, int = 0);
void initActorWithArchiveName(LiveActor* actor, const ActorInitInfo& info, const sead::SafeString& archiveName, const char* subArchiveName = nullptr);

void initCreateActorNoPlacementInfo(LiveActor* actor, const ActorInitInfo& info);
void initLinksActor(LiveActor* actor, const al::ActorInitInfo& info, const char* linkName, int index);

void initActorAudioKeeper(LiveActor* actor, const ActorInitInfo& info, const char* audioEntryName, const sead::Vector3f* pos3d, const sead::Matrix34f* mtx);
void initActorEffectKeeper(LiveActor* actor, const ActorInitInfo& info, const char* effectEntryName);
void initActorModelKeeper(LiveActor* actor, const ActorInitInfo& info, const char* modelArc, int, const char* animArc);
void initActorSceneInfo(LiveActor* actor, const ActorInitInfo& info);
void initNerve(LiveActor* actor, const Nerve* nerve, int bufSize = 0);
void initNerveAction(LiveActor* actor, const char* action, alNerveFunction::NerveActionCollector* collector, int bufSize);

void initExecutorMapObj(LiveActor* actor, const ActorInitInfo& info);
void initExecutorEnemyMovement(LiveActor* actor, const ActorInitInfo& info);
void initExecutorPlayerMovement(LiveActor* actor, const ActorInitInfo& info);

void trySyncStageSwitchAppear(LiveActor* actor);
void trySyncStageSwitchAppearAndKill(LiveActor* actor);

} // namespace al