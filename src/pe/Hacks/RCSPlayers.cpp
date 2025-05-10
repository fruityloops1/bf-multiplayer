#include "pe/Hacks/RCSPlayers.h"
#include "Game/Layout/RCSControlGuideBar.h"
#include "Game/Player/Player/PlayerFigureDirector.h"
#include "Game/Player/PlayerActor.h"
#include "Game/Player/PlayerAnimFunc.h"
#include "Game/Player/PlayerRetargettingSelector.h"
#include "Game/Scene/PhaseBossScene.h"
#include "Game/Scene/SingleModeScene.h"
#include "al/Base/String.h"
#include "al/Camera/CameraDirector.h"
#include "al/HitSensor/HitSensor.h"
#include "al/Layout/LayoutActor.h"
#include "al/Layout/LayoutInitInfo.h"
#include "al/LiveActor/ActorSensorFunction.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Memory/MemorySystem.h"
#include "al/Nerve/NerveFunction.h"
#include "al/Player/PlayerHolder.h"
#include "al/Sequence/Sequence.h"
#include "heap/seadExpHeap.h"
#include "heap/seadFrameHeap.h"
#include "helpers/fsHelper.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "nn/init.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Hooks.h"
#include "pe/Util/Log.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/Type.h"
#include "prim/seadSafeString.h"

static const char* getPlayerNameHook(int index) { return pe::util::sPlayerNames[(int)pe::RCSPlayers::getCurrentPlayerType()]; }

constexpr u32 cFightInitFix[] {
    0xD503201F,
    0xD503201F,
    0xD503201F,
    0xD503201F,
};

// need a bigger one
sead::FrameHeap* sceneResourceHeapCreateHook(size_t size, const sead::SafeString& name, sead::Heap* parent, s32 idk, sead::Heap::HeapDirection direction, bool something) {
    // 520 because yuzu fucking sucks
    return sead::FrameHeap::create(520 * 1024 * 1024, name, parent, idk, direction, something);
}

static al::LiveActor* getKoopaJrDemoPlayerActor(const al::IUseCamera* user) {
    al::LiveActor* koopaJrPtr;
    __asm("mov %0, x19"
        : "=r"(koopaJrPtr));

    return al::getPlayerActor(koopaJrPtr, 0);
}

al::HitSensor* getHitSensorKoopaJrDemo(al::LiveActor* actor, const char* name) {
    al::LiveActor* koopaJrPtr;
    __asm("mov %0, x19"
        : "=r"(koopaJrPtr));

    return al::getHitSensor(koopaJrPtr, name);
}

static void phaseBossPlayerFigureInit(al::LiveActor* actor, u32 figureType, bool something) {
    SingleModeScene* scene;
    __asm("mov %0, x19"
        : "=r"(scene));

    rc::initPlayerFigureType(actor, scene->isBossScene() ? (u32)EPlayerFigure::ClimbGiga : figureType, something);
}

static void bossStateDemoSkipLayoutInit(al::LayoutActor* actor, const al::LayoutInitInfo& info, bool something) {
    new (actor) al::LayoutActor("Dummy");
    al::initLayoutActor(actor, info, "RCS_SceneSkip");
    actor->kill();
}

constexpr u32 cDisableDarkBowserDemoSkip[] { 0x52800000 };
constexpr u32 cSingleModeActorKitCapacity[] { 0x528ABE03 };

HkTrampoline<int, sead::BufferedSafeString*, const sead::SafeString&, const sead::SafeString&, EPlayerFigure> playerModelNameCreateHook = hk::hook::trampoline([](sead::BufferedSafeString* out, const sead::SafeString& path, const sead::SafeString& playerName, EPlayerFigure figure) -> int {
    if (!al::isEqualString(playerName, "Mario") && figure == EPlayerFigure::ClimbGiga) {
        figure = EPlayerFigure::Climb;
    }

    return playerModelNameCreateHook.orig(out, path, playerName, figure);
});

constexpr u32 cMoreGpuMemAllocatorModelUBOMemory[] {
    0x52A07408,
    0x52A09C09,
};

constexpr u32 cMoreGpuMemAllocatorFreeListEntries[] {
    0x52A00023,
    0x531F7863,
    0x2A0303E2,
    0x2A0303E1,
};

constexpr u32 cMoreGpuMemAllocatorShaderOptionUBOMemory[] {
    0x52A02408,
    0x11440109,
    0x52A01A0A,
    0x115C010C,
};

static void setRCSControlGuideBarCharacterIcon(RCSControlGuideBar* thisPtr) {
    thisPtr->setCharacter(pe::util::getPlayerName(pe::RCSPlayers::getCurrentPlayerType()));
}

static void setRCSControlGuideBarCharacterIconFromStartAction(al::IUseLayoutAction* user) {
    al::startAction(user, al::StringTmp<64>("SetIcon_%s", pe::util::getPlayerName(pe::RCSPlayers::getCurrentPlayerType())).cstr(), "Player");
}

static void gigaBellCollectFix(void* thisPtr, PlayerActor* actor) {
    actor->mPlayer->mFigureDirector->forceChange(EPlayerFigure::ClimbGiga);
}

void pe::RCSPlayers::initHooks() {
    hk::hook::writeBranch(hk::ro::getMainModule(), 0x0040f660, getPlayerNameHook);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x00872f34, sceneResourceHeapCreateHook);
    hk::ro::getMainModule()->writeRo(0x0039f580, cFightInitFix, sizeof(cFightInitFix));
    hk::ro::getMainModule()->writeRo(0x00015ec0, cDisableDarkBowserDemoSkip, sizeof(cDisableDarkBowserDemoSkip));
    hk::ro::getMainModule()->writeRo(0x003e752c, cSingleModeActorKitCapacity, sizeof(cSingleModeActorKitCapacity));
    playerModelNameCreateHook.installAtOffset(hk::ro::getMainModule(), 0x00376ee0);

    hk::ro::getMainModule()->writeRo(0x00804858, cMoreGpuMemAllocatorModelUBOMemory, sizeof(cMoreGpuMemAllocatorModelUBOMemory));
    hk::ro::getMainModule()->writeRo(0x0080481c, cMoreGpuMemAllocatorFreeListEntries, sizeof(cMoreGpuMemAllocatorFreeListEntries));
    hk::ro::getMainModule()->writeRo(0x0080483c, cMoreGpuMemAllocatorShaderOptionUBOMemory, sizeof(cMoreGpuMemAllocatorShaderOptionUBOMemory));

    hk::ro::getMainModule()->writeRo(0x00872c68, 0xD503201F); // phase 2 bowser defeat crash fix
    hk::ro::getMainModule()->writeRo(0x0003ea64, 0xD503201F); // phase 3 init fix
    hk::ro::getMainModule()->writeRo(0x0003ea90, 0xD503201F); // "
    hk::ro::getMainModule()->writeRo(0x00371fe8, 0xD503201F); // fix PlayerAliveWatcherGroup checking some id for no reason
    // Patcher(0x003d746c).WriteInst(Nop()); // go right back into giga fight after death
    hk::ro::getMainModule()->writeRo(0x00319a00, 0xD65F03C0);
    hk::ro::getMainModule()->writeRo(0x003d6044, 0xD2A32000); // need more space for puppets in boss resource heap (300 -> 400 MB)
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003a6814, getKoopaJrDemoPlayerActor);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003a6a38, getKoopaJrDemoPlayerActor);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0039f62c, getHitSensorKoopaJrDemo);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x00015928, bossStateDemoSkipLayoutInit);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003e8344, phaseBossPlayerFigureInit); // fix not getting cat bell by default in giga fights for luigi-rosalina
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0017e594, setRCSControlGuideBarCharacterIcon);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x00176290, setRCSControlGuideBarCharacterIconFromStartAction);
    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0042eddc, gigaBellCollectFix);
}

pe::util::PlayerType& pe::RCSPlayers::getCurrentPlayerType() {
    static util::PlayerType type { util::PlayerType::Mario };
    return type;
}

static constexpr char sPlayerTypeFile[] = "sd:/BowsersFuryOnline/CurrentSelectedPlayer";

void pe::RCSPlayers::setPlayerType(util::PlayerType type) {
    getCurrentPlayerType() = type;
    FsHelper::writeFileToPath(&getCurrentPlayerType(), sizeof(pe::util::PlayerType), sPlayerTypeFile);
}

void pe::RCSPlayers::readPlayerTypeFromFile() {
    if (!FsHelper::isFileExist(sPlayerTypeFile)) {
        setPlayerType(util::PlayerType::Mario);
        return;
    }

    FsHelper::LoadData arg;
    arg.path = sPlayerTypeFile;
    FsHelper::loadFileFromPath(arg);
    getCurrentPlayerType() = *reinterpret_cast<util::PlayerType*>(arg.buffer);
    nn::init::GetAllocator()->Free(arg.buffer);
}