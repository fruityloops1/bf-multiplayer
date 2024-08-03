#include "pe/Hacks/Tweaks.h"
#include "al/Base/String.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Resource/Resource.h"
#include "al/Scene/LiveActorGroup.h"
#include "diag/assert.hpp"
#include "heap/seadFrameHeap.h"
#include "hook/trampoline.hpp"
#include "patch/code_patcher.hpp"
#include "pe/Util/Log.h"
#include <cstring>

namespace pe {

// this prevents crashes when using null strings in AnimPlayerSimple (no clue why this ever happens, tho)
static int animInfoTableStrcmpCheck(const char* a, const char* b)
{
    if (a == nullptr or b == nullptr) {
        return 1;
    }
    return strcmp(a, b);
}

static void liveActorGroupRegisterActorHook(al::LiveActorGroup* thisPtr, al::LiveActor* actor)
{
    EXL_ASSERT(thisPtr->mSize < thisPtr->mCapacity, "LiveActorGroup with name %s is full (%d)", thisPtr->mName, thisPtr->mCapacity);

    thisPtr->mActors[thisPtr->mSize] = actor;
    thisPtr->mSize++;
}

struct AllocFailedCallbackArg {
    sead::Heap* heap;
    size_t allocSize;
    u32 allocAlignment;
};

static void allocFailedReporter(void* thisPtr, const AllocFailedCallbackArg* arg)
{
    pe::err("Allocation of %zu bytes with alignment %u failed on heap %s with %d bytes free", arg->allocSize, arg->allocAlignment, arg->heap->getName().cstr(), arg->heap->getFreeSize());
    EXL_ABORT(42);
}

PATCH_DEFINE_ASM(DisableFirstAppearCutscene, R"(
    mov w0, #0
    ret
)")

PATCH_DEFINE_ASM(NoFadeToWhite, R"(
    mov w0, #0
    ret
)")

PATCH_DEFINE_ASM(SceneObjHolderSize, "mov w2, #0x3e");

static sead::FrameHeap* createSceneResourceHeapHook(size_t size, const sead::SafeString& name, sead::Heap* parent, s32 alignment, sead::Heap::HeapDirection direction, bool something)
{
    if (al::isEqualString(name, "SceneHeapResource") && size == 136314880 /* 130.0 MB */)
        size = 330 * 1024 * 1024; // need extra memory for all the puppets
    return sead::FrameHeap::tryCreate(size, name, parent, alignment, direction, something);
}

HOOK_DEFINE_TRAMPOLINE(FindOrCreateResourceWarn) { static al::Resource* Callback(const sead::SafeString& archive, const char* sth); };

al::Resource* FindOrCreateResourceWarn::Callback(const sead::SafeString& archive, const char* sth)
{
    al::Resource* resource = Orig(archive, sth);
    if (resource == nullptr)
        pe::warn("al::findOrCreateResource(\"%s\", %s) returned nullptr", archive.cstr(), sth ? sth : "nullptr");

    return resource;
}

HOOK_DEFINE_TRAMPOLINE(InitializeGfxMemoryPool) { static void Callback(void* memBlock, void* x1); };

void InitializeGfxMemoryPool::Callback(void* memBlock, void* x1)
{
    if (memBlock == nullptr)
        pe::err("GPU mem allocation failed");
    Orig(memBlock, x1);
}

void installTweaks()
{
    using Patcher = exl::patch::CodePatcher;
    Patcher(0x008d59ec).BranchLinkInst((void*)animInfoTableStrcmpCheck);
    Patcher(0x008316c0).BranchInst((void*)liveActorGroupRegisterActorHook);
    Patcher(0x00872c30).BranchInst((void*)allocFailedReporter);
    NoFadeToWhite::InstallAtOffset(0x0041d3f0);
    DisableFirstAppearCutscene::InstallAtOffset(0x0020d9c0);
    Patcher(0x00720554).BranchInst((void*)createSceneResourceHeapHook);
    SceneObjHolderSize::InstallAtOffset(0x003e624c);
    FindOrCreateResourceWarn::InstallAtOffset(0x008a0270);
    InitializeGfxMemoryPool::InstallAtOffset(0x00758710);
}

} // namespace pe
