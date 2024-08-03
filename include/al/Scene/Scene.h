#pragma once

#include "al/Audio/AudioKeeper.h"
#include "al/Nerve/NerveExecutor.h"
#include "al/Scene/LiveActorKit.h"
#include "al/Scene/SceneInitInfo.h"
#include "al/Scene/SceneObjHolder.h"
#include <sead/prim/seadSafeString.h>

namespace al {

class Scene : public NerveExecutor, public IUseAudioKeeper, public IUseCamera, public IUseSceneObjHolder {
public:
    sead::FixedSafeString<64> mName;
    bool mIsAlive = false;
    bool mIsControlDead = false;
    class StageResourceKeeper* mStageResourceKeeper = nullptr;
    LiveActorKit* mLiveActorKit = nullptr;
    class LayoutKit* mLayoutKit = nullptr;
    SceneObjHolder* mSceneObjHolder = nullptr;
    class SceneStopCtrl* mSceneStopCtrl = nullptr;
    class AudioDirector* mAudioDirector = nullptr;
    AudioKeeper* mAudioKeeper = nullptr;
    class ScreenCoverCtrl* mScreenCoverCtrl = nullptr;
    class DrawSystemInfo* mDrawSystemInfo = nullptr;

    void* unk_d0 = nullptr;
    void* unk_d8 = nullptr;
    u8 unk_e0;

    virtual ~Scene();
    virtual void init(const SceneInitInfo& info);
    virtual void appear();
    virtual void kill();
    virtual void movement();
    virtual void control();
    AudioKeeper* getAudioKeeper() const override;
    SceneObjHolder* getSceneObjHolder() const override;
    SceneCameraInfo* getSceneCameraInfo() const override;

private:
    virtual bool isValidPlacementParent(const PlacementInfo& info) const;
    virtual bool isValidPlacement(const PlacementInfo& info) const;

public:
    virtual void drawMain_() const;
    virtual void drawSub_() const;
};

} // namespace al
