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
#include "hook/trampoline.hpp"
#include "lib.hpp"
#include "nn/init.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Hooks.h"
#include "pe/Util/Log.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/Type.h"
#include "prim/seadSafeString.h"

static const char* getPlayerNameHook(int index) { return pe::util::sPlayerNames[(int)pe::RCSPlayers::getCurrentPlayerType()]; }

PATCH_DEFINE_ASM(FightInitFix, R"(
    nop
    nop
    nop
    nop
)");

// need a bigger one
sead::FrameHeap* sceneResourceHeapCreateHook(size_t size, const sead::SafeString& name, sead::Heap* parent, s32 idk, sead::Heap::HeapDirection direction, bool something)
{
    // 520 because yuzu fucking sucks
    return sead::FrameHeap::create(520 * 1024 * 1024, name, parent, idk, direction, something);
}

static al::LiveActor* getKoopaJrDemoPlayerActor(const al::IUseCamera* user)
{
    al::LiveActor* koopaJrPtr;
    __asm("mov %0, x19"
          : "=r"(koopaJrPtr));

    return al::getPlayerActor(koopaJrPtr, 0);
}

al::HitSensor* getHitSensorKoopaJrDemo(al::LiveActor* actor, const char* name)
{
    al::LiveActor* koopaJrPtr;
    __asm("mov %0, x19"
          : "=r"(koopaJrPtr));

    return al::getHitSensor(koopaJrPtr, name);
}

static void phaseBossPlayerFigureInit(al::LiveActor* actor, u32 figureType, bool something)
{
    SingleModeScene* scene;
    __asm("mov %0, x19"
          : "=r"(scene));

    rc::initPlayerFigureType(actor, scene->isBossScene() ? (u32)EPlayerFigure::ClimbGiga : figureType, something);
}

static void bossStateDemoSkipLayoutInit(al::LayoutActor* actor, const al::LayoutInitInfo& info, bool something)
{
    new (actor) al::LayoutActor("Dummy");
    al::initLayoutActor(actor, info, "RCS_SceneSkip");
    actor->kill();
}

PATCH_DEFINE_ASM(DisableDarkBowserDemoSkip, "mov w0, #0");
PATCH_DEFINE_ASM(SingleModeActorKitCapacity, "mov w3, #22000");

HOOK_DEFINE_TRAMPOLINE(PlayerModelNameCreate) { static int Callback(sead::BufferedSafeString * out, const sead::SafeString& path, const sead::SafeString& playerName, EPlayerFigure figure); };

int PlayerModelNameCreate::Callback(sead::BufferedSafeString* out, const sead::SafeString& path, const sead::SafeString& playerName, EPlayerFigure figure)
{
    if (!al::isEqualString(playerName, "Mario") && figure == EPlayerFigure::ClimbGiga) {
        figure = EPlayerFigure::Climb;
    }

    return Orig(out, path, playerName, figure);
}

PATCH_DEFINE_ASM(MoreGpuMemAllocatorModelUBOMemory, R"(
    mov w8, #0x3a00000
    mov w9, #0x4e00000
)")

PATCH_DEFINE_ASM(MoreGpuMemAllocatorFreeListEntries, R"(
    mov w3, #0x10000
    lsl w3, w3, #1
    mov w2, w3
    mov w1, w3
)")

PATCH_DEFINE_ASM(MoreGpuMemAllocatorShaderOptionUBOMemory, R"(
    mov w8, #0x1200000
    add w9, w8, #0x100000
    mov w10, #0xd00000
    add w12, w8, #0x700000
)")

static void setRCSControlGuideBarCharacterIcon(RCSControlGuideBar* thisPtr)
{
    thisPtr->setCharacter(pe::util::getPlayerName(pe::RCSPlayers::getCurrentPlayerType()));
}

static void setRCSControlGuideBarCharacterIconFromStartAction(al::IUseLayoutAction* user)
{
    al::startAction(user, al::StringTmp<64>("SetIcon_%s", pe::util::getPlayerName(pe::RCSPlayers::getCurrentPlayerType())).cstr(), "Player");
}

static void gigaBellCollectFix(void* thisPtr, PlayerActor* actor)
{
    actor->mPlayer->mFigureDirector->forceChange(EPlayerFigure::ClimbGiga);
}

void pe::RCSPlayers::initHooks()
{
    using Patcher = exl::patch::CodePatcher;
    using namespace exl::armv8::inst;

    Patcher(0x0040f660).BranchInst((void*)getPlayerNameHook);
    Patcher(0x00872f34).BranchLinkInst((void*)sceneResourceHeapCreateHook);
    FightInitFix::InstallAtOffset(0x0039f580);
    DisableDarkBowserDemoSkip::InstallAtOffset(0x00015ec0);
    SingleModeActorKitCapacity::InstallAtOffset(0x003e752c);
    PlayerModelNameCreate::InstallAtOffset(0x00376ee0);

    MoreGpuMemAllocatorModelUBOMemory::InstallAtOffset(0x00804858);
    MoreGpuMemAllocatorFreeListEntries::InstallAtOffset(0x0080481c);
    MoreGpuMemAllocatorShaderOptionUBOMemory::InstallAtOffset(0x0080483c);

    Patcher(0x00872c68).WriteInst(Nop()); // phase 2 bowser defeat crash fix
    Patcher(0x0003ea64).WriteInst(Nop()); // phase 3 init fix
    Patcher(0x0003ea90).WriteInst(Nop()); // "
    Patcher(0x00371fe8).WriteInst(Nop()); // fix PlayerAliveWatcherGroup checking some id for no reason
    // Patcher(0x003d746c).WriteInst(Nop()); // go right back into giga fight after death
    Patcher(0x00319a00).WriteInst(Ret());
    Patcher(0x003d6044).Write(0xD2A32000); // need more space for puppets in boss resource heap (300 -> 400 MB)
    Patcher(0x003a6814).BranchLinkInst((void*)getKoopaJrDemoPlayerActor);
    Patcher(0x003a6a38).BranchLinkInst((void*)getKoopaJrDemoPlayerActor);
    Patcher(0x0039f62c).BranchLinkInst((void*)getHitSensorKoopaJrDemo);
    Patcher(0x00015928).BranchLinkInst((void*)bossStateDemoSkipLayoutInit);
    Patcher(0x003e8344).BranchLinkInst((void*)phaseBossPlayerFigureInit); // fix not getting cat bell by default in giga fights for luigi-rosalina
    Patcher(0x0017e594).BranchLinkInst((void*)setRCSControlGuideBarCharacterIcon);
    Patcher(0x00176290).BranchLinkInst((void*)setRCSControlGuideBarCharacterIconFromStartAction);
    Patcher(0x0042eddc).BranchLinkInst((void*)gigaBellCollectFix);
}

pe::util::PlayerType& pe::RCSPlayers::getCurrentPlayerType()
{
    static util::PlayerType type { util::PlayerType::Mario };
    return type;
}

static constexpr char sPlayerTypeFile[] = "sd:/BowsersFuryOnline/CurrentSelectedPlayer";

void pe::RCSPlayers::setPlayerType(util::PlayerType type)
{
    getCurrentPlayerType() = type;
    FsHelper::writeFileToPath(&getCurrentPlayerType(), sizeof(pe::util::PlayerType), sPlayerTypeFile);
}

void pe::RCSPlayers::readPlayerTypeFromFile()
{
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