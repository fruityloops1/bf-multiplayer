#include "pe/EchoEmitterMod.h"
#include "Game/MapObj/EchoBlockMapParts.h"
#include "Game/Scene/StageScene.h"
#include "al/Audio/MeInfo.h"
#include "al/LiveActor/ActorSceneFunction.h"
#include "al/Scene/SceneInitInfo.h"
#include "al/Se/SeFunction.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include <sead/container/seadPtrArray.h>
#include <sead/prim/seadSafeString.h>

static sead::PtrArray<EchoBlockMapParts> allEchoBlocks;

HkTrampoline<void, EchoBlockMapParts*, const al::ActorInitInfo&> echoBlockMapPartsInitHook = hk::hook::trampoline([](EchoBlockMapParts* actor, const al::ActorInitInfo& info) {
    echoBlockMapPartsInitHook.orig(actor, info);
    allEchoBlocks.pushBack(actor);
});

HkTrampoline<void, StageScene*, al::SceneInitInfo*> stageSceneInitHook = hk::hook::trampoline([](StageScene* scene, al::SceneInitInfo* info) {
    if (allEchoBlocks.isBufferReady())
        allEchoBlocks.freeBuffer();
    allEchoBlocks.allocBuffer(15, nullptr);
    stageSceneInitHook.orig(scene, info);
});

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
    echoBlockMapPartsInitHook.installAtOffset(hk::ro::getMainModule(), 0x002278e0);
    stageSceneInitHook.installAtOffset(hk::ro::getMainModule(), 0x003f11d0);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x002282b8, startSeHook);
}