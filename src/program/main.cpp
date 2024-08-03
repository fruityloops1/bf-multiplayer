#include "Game/Scene/SingleModeScene.h"
#include "Game/Scene/StageScene.h"
#include "al/Base/String.h"
#include "al/Controller/ControllerUtil.h"
#include "hook/trampoline.hpp"
#include "lib.hpp"
#include "nn/socket.h"
#include "pe/BunbunMod.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/DbgGui/Windows/Multiplayer.h"
#include "pe/EchoEmitterMod.h"
#include "pe/Exception.h"
#include "pe/Execute/ExecuteDirectorExtensions.h"
#include "pe/Factory/ProjectActorFactory.h"
#include "pe/Hacks/FSHacks.h"
#include "pe/Hacks/PlacementHolderMod.h"
#include "pe/Hacks/RCSPlayers.h"
#include "pe/Hacks/TopMenuSceneMod.h"
#include "pe/Hacks/Tweaks.h"
#include "pe/Multiplayer/DisasterSync.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Multiplayer/SaveDataSync.h"
#include "pe/Util/Log.h"

HOOK_DEFINE_TRAMPOLINE(StageSceneMovementHook) { static void Callback(StageScene*); };
void StageSceneMovementHook::Callback(StageScene* scene) { Orig(scene); }

HOOK_DEFINE_TRAMPOLINE(SingleModeSceneDtorHook) { static void Callback(SingleModeScene*); };
void SingleModeSceneDtorHook::Callback(SingleModeScene* scene)
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    mgr->destroyStatusText();
    Orig(scene);
}

extern "C" void exl_main(void* x0, void* x1)
{
    exl::hook::Initialize();

    using Patcher = exl::patch::CodePatcher;
    // using namespace exl::patch::inst;

    {
        constexpr size_t poolSize = 0xC0000;
        void* pool = malloc(poolSize);
        nn::socket::Initialize(pool, poolSize, 0x4000, 0xe);
    }

    StageSceneMovementHook::InstallAtOffset(0x003f2d20);
    SingleModeSceneDtorHook::InstallAtOffset(0x003e6810);

    pe::initUserExceptionHandler();
    pe::installFSHacks();
    pe::initProjectActorFactoryHook();
    pe::initBunbunModHooks();
    pe::initEchoEmitterModHooks();
    pe::initPlacementHolderModHooks();
    pe::initExecuteDirectorExtensionHooks();
    pe::installTweaks();
    pe::RCSPlayers::initHooks();
    pe::installTopMenuSceneMod();
    pe::installDisasterSyncHooks();
    pe::gui::initDbgGuiHooks();
    // pe::initWiiUModHooks();
    pe::installSaveDataSyncHooks();

    exl::util::RwPages a(exl::util::GetSdkModuleInfo().m_Total.m_Start + 0x00338554, 4);
    *reinterpret_cast<u32*>(a.GetRw()) = 0xD65F03C0; // just dont crash, idiot
}

extern "C" NORETURN void exl_exception_entry()
{
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
