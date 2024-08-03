#pragma once

#include "Game/Scene/LuigiBrosScene.h"
#include "Game/Sequence/StageWipeKeeper.h"
#include "Game/System/GameDataHolder.h"
#include "al/Nerve/HostStateBase.h"
#include "al/Scene/SceneCreator.h"
#include "al/Screen/ScreenCaptureExecutor.h"
#include "al/Sequence/SequenceInitInfo.h"

class ProductSequence;
class ProductStateLuigiBros : public al::HostStateBase<ProductSequence>, public al::IUseSceneCreator {
    al::SceneCreator* mSceneCreator = nullptr;
    StageWipeKeeper* mWipeKeeper = nullptr;
    LuigiBrosScene* mScene = nullptr;
    void* _40 = nullptr;
    void* _48 = nullptr;

public:
    ProductStateLuigiBros(ProductSequence* host, void* unk1, const al::SequenceInitInfo& info, void* unk2, GameDataHolder* gameDataHolder, StageWipeKeeper* stageWipeKeeper, al::ScreenCaptureExecutor* screenCaptureExecutor);

    void init() override;
    void appear() override;
    al::SceneCreator* getSceneCreator() const override { return mSceneCreator; }
    void setSceneCreator(al::SceneCreator* p) override { mSceneCreator = p; }

    void exeLoad();
    void exeWait();
};
