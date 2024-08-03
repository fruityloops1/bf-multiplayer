#include "pe/EchoEmitterMod.h"
#include "Game/MapObj/EchoBlockMapParts.h"
#include "Game/Scene/StageScene.h"
#include "al/Audio/MeInfo.h"
#include "al/LiveActor/ActorSceneFunction.h"
#include "al/Scene/SceneInitInfo.h"
#include "al/Se/SeFunction.h"
#include "lib.hpp"
#include <sead/container/seadPtrArray.h>
#include <sead/prim/seadSafeString.h>

HOOK_DEFINE_TRAMPOLINE(EchoBlockMapPartsInitHook) { static void Callback(EchoBlockMapParts*, const al::ActorInitInfo&); };
HOOK_DEFINE_TRAMPOLINE(StageSceneInitHook) { static void Callback(StageScene*, al::SceneInitInfo*); };

static sead::PtrArray<EchoBlockMapParts> allEchoBlocks;

void EchoBlockMapPartsInitHook::Callback(EchoBlockMapParts* actor, const al::ActorInitInfo& info)
{
    Orig(actor, info);
    allEchoBlocks.pushBack(actor);
}

void StageSceneInitHook::Callback(StageScene* scene, al::SceneInitInfo* info)
{
    if (allEchoBlocks.isBufferReady())
        allEchoBlocks.freeBuffer();
    allEchoBlocks.allocBuffer(15, nullptr);
    Orig(scene, info);
}

static void startSeHook(al::IUseAudioKeeper* audioKeeper, const sead::SafeString& seName, al::MeInfo* info)
{
    al::LiveActor* actor = static_cast<al::LiveActor*>(audioKeeper);
    bool startSound = al::isSingleMode(actor);
    for (EchoBlockMapParts& actor : allEchoBlocks)
        if (!al::isClipped(&actor))
            startSound = true;
    if (startSound)
        al::startSe(audioKeeper, seName, info);
}

void pe::initEchoEmitterModHooks()
{
    using Patcher = exl::patch::CodePatcher;

    EchoBlockMapPartsInitHook::InstallAtOffset(0x002278e0);
    StageSceneInitHook::InstallAtOffset(0x003f11d0);
    Patcher(0x002282b8).BranchLinkInst((void*)startSeHook);
}