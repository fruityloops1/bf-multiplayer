#include "pe/BunbunMod.h"
#include "Game/Boss/Bunbun.h"
#include "Game/Boss/BunbunStateShellAttack.h"
#include "al/Collision/CollisionParts.h"
#include "al/LiveActor/ActorParamHolder.h"
#include "al/Nerve/NerveFunction.h"
#include "lib.hpp"
#include "pe/Util/Hooks.h"
#include "pe/Util/Nerve.h"

HOOK_DEFINE_TRAMPOLINE(BunbunStateShellAttackChangeShellHook) { static void Callback(BunbunStateShellAttack*); };
HOOK_DEFINE_TRAMPOLINE(BunbunStateShellAttackRecoverShellHook) { static void Callback(BunbunStateShellAttack*); };
HOOK_DEFINE_TRAMPOLINE(BunbunStateShellAttackSpinHook) { static void Callback(BunbunStateShellAttack*); };
HOOK_DEFINE_TRAMPOLINE(BunbunStateSpinAttackAttackHook) { static void Callback(BunbunStateSpinAttack*); };

void BunbunStateShellAttackChangeShellHook::Callback(BunbunStateShellAttack* state)
{
    if (al::isFirstStep(state))
        al::tryOnStageSwitch(state->mBunbun, "SwitchChangeShellOn");

    Orig(state);
}

void BunbunStateShellAttackRecoverShellHook::Callback(BunbunStateShellAttack* state)
{

    if (al::isFirstStep(state))
        al::tryOffStageSwitch(state->mBunbun, "SwitchChangeShellOn");

    Orig(state);
}

void BunbunStateShellAttackSpinHook::Callback(BunbunStateShellAttack* state)
{
    Orig(state); // copium
}

void BunbunStateSpinAttackAttackHook::Callback(BunbunStateSpinAttack* state)
{
    Orig(state); // immense copium
}

void pe::initBunbunModHooks()
{
    BunbunStateShellAttackChangeShellHook::InstallAtOffset(0x0000e7b0);
    BunbunStateShellAttackRecoverShellHook::InstallAtOffset(0x0000ef80);
    // BunbunStateShellAttackSpinHook::InstallAtOffset(0x0000ea80);
    // BunbunStateSpinAttackAttackHook::InstallAtOffset(0x0000f7e0);
}