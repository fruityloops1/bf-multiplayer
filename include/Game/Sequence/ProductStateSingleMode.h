#pragma once

#include "al/Nerve/HostStateBase.h"
#include "al/Scene/SceneCreator.h"
#include "al/Sequence/SequenceInitInfo.h"
#include <sead/basis/seadTypes.h>

class ProductSequence;
class ProductStateSingleMode : public al::HostStateBase<ProductSequence>, public al::IUseSceneCreator {
private:
    struct ProductStageStartParam* mStartParam;
    void* unk1[2];
    al::SequenceInitInfo* mSequenceInitInfo;
    void* unk2;
    al::SceneCreator* mSceneCreator;
    int _58;
    int mStageIndex = 0;

public:
    void init() override;
    void appear() override;
    void kill() override;
    al::SceneCreator* getSceneCreator() const override;
    void setSceneCreator(al::SceneCreator*) override;
};