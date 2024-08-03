#pragma once

#include "al/Audio/AudioDirector.h"
#include "al/Audio/AudioKeeper.h"
#include "al/Nerve/NerveExecutor.h"
#include "al/Scene/Scene.h"
#include "al/Scene/SceneCreator.h"
#include "al/Sequence/SequenceInitInfo.h"
#include "nerd/ErrorViewer.h"

namespace al {

class StageInfo;

class Sequence : public NerveExecutor, public IUseAudioKeeper, public IUseSceneCreator {
public:
    sead::FixedSafeString<64> mName;
    al::Scene* mCurrentScene = nullptr;
    al::Scene* mCurrentInitScene = nullptr;
    SceneCreator* mSceneCreator = nullptr;
    AudioDirector* mAudioDirector = nullptr;
    AudioKeeper* mAudioKeeper = nullptr;
    void* unk2;
    bool mIsAlive = true;

    ~Sequence() override;
    virtual void init(const SequenceInitInfo& info);
    virtual void update();
    virtual void kill();
    virtual void drawMain();
    virtual void drawSub();
    AudioKeeper* getAudioKeeper() const override;
    virtual bool isDisposable() const;
    virtual Scene* getCurrentScene() const;

private:
    ErrorViewer* getErrorViewer() const;

public:
    SceneCreator* getSceneCreator() const override;
    void setSceneCreator(SceneCreator*) override;

    AudioDirector* getAudioDirector() const { return mAudioDirector; }

    void setCurrentScene(al::Scene* scene) { mCurrentScene = scene; }

    al::Scene* tryGetCurrentScene() const
    {
        return mCurrentInitScene ? mCurrentInitScene : mCurrentScene;
    }
};

} // namespace al