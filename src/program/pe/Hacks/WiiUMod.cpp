#include "pe/Hacks/WiiUMod.h"
#include "Game/Sequence/ProductSequence.h"
#include "Game/System/AssetLoadingThread.h"
#include "Game/Util/PlayerConstUtil.h"
#include "al/Layout/LayoutActor.h"
#include "al/Memory/MemorySystem.h"
#include "al/Npc/SimpleLayoutAppearWaitEnd.h"
#include "filedevice/seadFileDevice.h"
#include "heap/seadFrameHeap.h"
#include "hook/trampoline.hpp"
#include "nn/oe.h"
#include "patch/code_patcher.hpp"
#include "pe/Util/Hooks.h"
#include "pe/Util/Nerve.h"
#include <heap/seadHeap.h>

namespace pe {

HOOK_DEFINE_REPLACE(DemoSkipLayoutStub) { static bool Callback(al::LayoutActor*, sead::BitFlag16); };

bool DemoSkipLayoutStub::Callback(al::LayoutActor*, sead::BitFlag16)
{
    return false;
}

static void initProductSequenceNrvHook(ProductSequence* sequence, const al::Nerve*, int numNerveStates)
{
    sequence->initNerve(pe::util::getNerveAt(0x0138cf70), numNerveStates);
}

static void initWindow(al::LayoutActor* layout, const char* pName, const char* pLayoutName, const al::LayoutInitInfo& pInfo, const char*, bool pIsLocalized)
{
    new (layout) al::SimpleLayoutAppearWaitEnd(pName, "HeadMiss", pInfo, nullptr, pIsLocalized);
}

static void createSceneResourceHeapForTitleSceneHook9(const char* stageName)
{ // title wants more
    al::createSceneHeap("SingleModeOceanPhase0Stage"); // random entry with 200mb
}

static sead::FrameHeap* createCourseSelectResourceHeapHook(size_t size, const sead::SafeString& name, sead::Heap* parent, size_t smth, sead::FrameHeap::HeapDirection dir, bool smth2)
{ // increase course select resource heap size (ive LICHAARALLEY got no clue why it wants more now.)
    return sead::FrameHeap::create(0x10000000, name, parent, smth, dir, smth2);
}

static void* newCallback(size_t size, sead::Heap* heap, int alignment)
{
    // pe::log("Allocating 0x%.8x with 0x%.8x left out of 0x%.8x %s (alignment: %d) ", size, heap->getFreeSize(), heap->getSize(), heap->getName().cstr(), alignment);
    return new (heap, alignment) u8[size];
}

PATCH_DEFINE_ASM(DefaultSceneResourceSize, "mov w8,#0x0c800000")

static bool productStateBootWaitInitHook(al::IUseNerve* state)
{
    bool isFirstStep = al::isFirstStep(state);

    if (isFirstStep) {
        rc::AssetLoadingThread* thread = rc::AssetLoadingThread::instance();
        thread->disableFastLoad(false);
        thread->startLoad(rc::AssetLoadingThread::LOAD_TYPE::Something2);

        nn::oe::FinishStartupLogo();
    }

    return isFirstStep;
}

HOOK_DEFINE_TRAMPOLINE(abc) { static void Callback(al::Scene * scene); };
void abc::Callback(al::Scene* scene)
{
    Orig(scene);
    // pe::log("BootScene::tryEnd");
}

void initWiiUModHooks()
{
    using Patcher = exl::patch::CodePatcher;
    using namespace exl::patch::inst;

    // DefaultSceneResourceSize::InstallAtOffset(0x00872f08);

    abc::InstallAtOffset(0x003d09b0);

    Patcher(0x0074b0d4).BranchLinkInst((void*)newCallback);

    // Patcher(0x0040404c).BranchLinkInst((void*)createSceneResourceHeapForTitleSceneHook);
    // Patcher(0x0087306c).BranchLinkInst((void*)createCourseSelectResourceHeapHook);

    DemoSkipLayoutStub::InstallAtOffset(0x00141b00);

    rc::setUsingOldPlayerParams(true);
    Patcher(0x003f80e4).Write(Nop()); // disable player params being set to new
    Patcher(0x003f8184).Write(Nop()); // "

    Patcher(0x003fcb28).Write(Nop()); // start with boot state instead of top menu state
    Patcher(0x003fcb10).BranchLinkInst((void*)initProductSequenceNrvHook);

    // change all references from TopMenu to Title
    Patcher(0x003fd39c).BranchInst((void*)pe::util::setNerveHook<0x0138cf70>);
    Patcher(0x003fd774).BranchInst((void*)pe::util::setNerveHook<0x0138cf70>);
    Patcher(0x003fda38).BranchInst((void*)pe::util::setNerveHook<0x0138cf70>);
    Patcher(0x003fd4e0).BranchInst((void*)pe::util::setNerveHookIfNot<0x0138cf70, 0x0138cfa8>);

    Patcher(0x003fe784).BranchLinkInst((void*)productStateBootWaitInitHook);
    Patcher(0x003fe798).Write(Nop());

    Patcher(0x003d0840).BranchLinkInst((void*)initWindow); // boot window
    Patcher(0x003d0870).BranchLinkInst((void*)initWindow); // controller window
}

} // namespace pe
