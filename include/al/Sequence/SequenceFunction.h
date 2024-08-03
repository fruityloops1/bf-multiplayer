#pragma once

#include "al/Audio/AudioDirector.h"
#include "al/Scene/Scene.h"
#include "al/Scene/SceneCreator.h"
#include "al/Scene/SceneFunction.h"
#include "al/Screen/ScreenCaptureExecutor.h"
#include "al/Sequence/SequenceInitInfo.h"
#include "al/System/GameDataHolderBase.h"

namespace al {

void initSceneCreator(IUseSceneCreator* creator, const SequenceInitInfo& info, GameDataHolderBase*, AudioDirector*, ScreenCaptureExecutor*, alSceneFunction::SceneFactory*);
void setSceneAndUseInitThread(IUseSceneCreator* creator, Scene* scene, int, const char*, int, const char*, sead::Heap* heap);
bool tryEndSceneInitThread(IUseSceneCreator* creator);

} // namespace al
