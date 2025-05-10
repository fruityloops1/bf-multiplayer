#include "pe/Hacks/WiiUMod.h"
#include "Game/Sequence/ProductSequence.h"
#include "Game/System/AssetLoadingThread.h"
#include "Game/Util/PlayerConstUtil.h"
#include "al/Layout/LayoutActor.h"
#include "al/Memory/MemorySystem.h"
#include "al/Npc/SimpleLayoutAppearWaitEnd.h"
#include "filedevice/seadFileDevice.h"
#include "heap/seadFrameHeap.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "nn/oe.h"
#include "pe/Util/Hooks.h"
#include "pe/Util/Nerve.h"
#include <heap/seadHeap.h>

namespace pe {

HkReplace<bool, al::LayoutActor*, sead::BitFlag16> demoSkipLayoutStubHook = hk::hook::replace([](al::LayoutActor* layout, sead::BitFlag16 flag) {
    return false;
});

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

constexpr u32 cDefaultSceneResourceSize[] {
    0x52A19008,
};

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

void initWiiUModHooks()
{
    // DefaultSceneResourceSize::InstallAtOffset(0x00872f08);

    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0074b0d4, newCallback);

    // Patcher(0x0040404c).BranchLinkInst((void*)createSceneResourceHeapForTitleSceneHook);
    // Patcher(0x0087306c).BranchLinkInst((void*)createCourseSelectResourceHeapHook);

    demoSkipLayoutStubHook.installAtOffset(hk::ro::getMainModule(), 0x00141b00);

    rc::setUsingOldPlayerParams(true);
    hk::ro::getMainModule()->writeRo(0x003f80e4, 0xD503201F); // disable player params being set to new
    hk::ro::getMainModule()->writeRo(0x003f8184, 0xD503201F); // "

    hk::ro::getMainModule()->writeRo(0x003fcb28, 0xD503201F); // start with boot state instead of top menu state
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003fcb10, initProductSequenceNrvHook);

    // change all references from TopMenu to Title
    hk::hook::writeBranch(hk::ro::getMainModule(), 0x003fd39c, pe::util::setNerveHook<0x0138cf70>);
    hk::hook::writeBranch(hk::ro::getMainModule(), 0x003fd774, pe::util::setNerveHook<0x0138cf70>);
    hk::hook::writeBranch(hk::ro::getMainModule(), 0x003fda38, pe::util::setNerveHook<0x0138cf70>);
    hk::hook::writeBranch(hk::ro::getMainModule(), 0x003fd4e0, pe::util::setNerveHookIfNot<0x0138cf70, 0x0138cfa8>);

    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003fe784, productStateBootWaitInitHook);
    hk::ro::getMainModule()->writeRo(0x003fe798, 0xD503201F);

    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003d0840, initWindow); // boot window
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003d0870, initWindow); // controller window
}

} // namespace pe
