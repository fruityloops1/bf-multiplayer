#include "pe/Hacks/PlacementHolderMod.h"
#include "Game/Scene/SingleModeScene.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Placement/PlacementHolder.h"
#include "heap/seadHeapMgr.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Util/Log.h"
#include <sead/basis/seadNew.h>

static void deletePlacementHolderIfExist(al::LiveActor* actor)
{
    al::PlacementHolder* placementHolder = actor->getPlacementHolder();
    if (placementHolder)
        delete placementHolder;
}

al::PlacementHolder::~PlacementHolder()
{
    if (pe::getPlacementInfoHeap()) {
        pe::getPlacementInfoHeap()->destroy();
        pe::getPlacementInfoHeap() = nullptr;
    }
}

HkTrampoline<void, SingleModeScene*> singleModeScenePrepareDestroyHook = hk::hook::trampoline([](SingleModeScene* scene) {
    singleModeScenePrepareDestroyHook.orig(scene);

    if (pe::getPlacementInfoHeap()) {
        pe::getPlacementInfoHeap()->destroy();
        pe::getPlacementInfoHeap() = nullptr;
    }
});

HkTrampoline<void, al::LiveActor*, void*> liveActorDtor1Hook = hk::hook::trampoline([](al::LiveActor* actor, void* vtt) {
    liveActorDtor1Hook.orig(actor, vtt);
    deletePlacementHolderIfExist(actor);
});

HkTrampoline<void, al::LiveActor*> liveActorDtor2Hook = hk::hook::trampoline([](al::LiveActor* actor) {
    liveActorDtor2Hook.orig(actor);
    deletePlacementHolderIfExist(actor);
});

HkTrampoline<void, al::LiveActor*> liveActorDtor3Hook = hk::hook::trampoline([](al::LiveActor* actor) {
    liveActorDtor3Hook.orig(actor);
    deletePlacementHolderIfExist(actor);
});

HkTrampoline<void, al::PlacementHolder*> placementHolderCtorHook = hk::hook::trampoline([](al::PlacementHolder* holder) {
    placementHolderCtorHook.orig(holder);
    holder->mUnitConfigName = nullptr;
    holder->mIdClone = nullptr;
    holder->mModelNameClone = nullptr;
    holder->mKilledByHook = false;
});

HkTrampoline<void, al::PlacementHolder*, const al::PlacementInfo&> placementHolderInitHook = hk::hook::trampoline([](al::PlacementHolder* holder, const al::PlacementInfo& info) -> void {
    placementHolderInitHook.orig(holder, info);

    if (pe::gui::DbgGui::instance()->isSingleModeScene())
        return;

    if (!pe::getPlacementInfoHeap()) {
        pe::createPlacementInfoHeap();
    }
    sead::ScopedCurrentHeapSetter setter(pe::getPlacementInfoHeap());

    const char* unitConfigName = nullptr;
    if (info.getPlacementIter().tryGetStringByKey(&unitConfigName, "UnitConfigName")) {
        size_t size = strlen(unitConfigName) + 1;
        char* out = new char[size];
        holder->mUnitConfigName = out;
        strcpy(out, unitConfigName);
    }

    const char* id = nullptr;
    if (info.getPlacementIter().tryGetStringByKey(&id, "Id")) {
        size_t size = strlen(id) + 1;
        char* out = new char[size];
        holder->mIdClone = out;
        strcpy(out, id);
    }

    const char* modelName = nullptr;
    if (info.getPlacementIter().tryGetStringByKey(&modelName, "ModelName")) {
        size_t size = strlen(modelName) + 1;
        char* out = new char[size];
        holder->mModelNameClone = out;
        strcpy(out, modelName);
    }
});

namespace pe {

sead::FrameHeap*& getPlacementInfoHeap()
{
    static sead::FrameHeap* heap { nullptr };
    return heap;
}

void createPlacementInfoHeap()
{
    getPlacementInfoHeap() = sead::FrameHeap::create(1024 * 1024 * 0.5, "PlacementInfoHeap", pe::gui::getPeepaHeap(), 8, sead::ExpHeap::cHeapDirection_Forward, false);
}

void initPlacementHolderModHooks()
{
    singleModeScenePrepareDestroyHook.installAtOffset(hk::ro::getMainModule(), 0x003e9460);
    hk::ro::getMainModule()->writeRo(0x0085b4d4, 0x52800900); // mov w0, #0x48
    hk::ro::getMainModule()->writeRo(0x0085b5ac, 0x52800900); // mov w0, #0x48
    liveActorDtor1Hook.installAtOffset(hk::ro::getMainModule(), 0x0085b5d0);
    liveActorDtor2Hook.installAtOffset(hk::ro::getMainModule(), 0x0085b680);
    liveActorDtor3Hook.installAtOffset(hk::ro::getMainModule(), 0x0085b720);
    placementHolderCtorHook.installAtOffset(hk::ro::getMainModule(), 0x00984150);
    placementHolderInitHook.installAtOffset(hk::ro::getMainModule(), 0x00984170);
}

} // namespace pe
