#include "pe/BunbunMod.h"
#include "Game/Boss/Bunbun.h"
#include "Game/Boss/BunbunStateShellAttack.h"
#include "al/Collision/CollisionParts.h"
#include "al/LiveActor/ActorParamHolder.h"
#include "al/Nerve/NerveFunction.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "pe/Util/Nerve.h"

HkTrampoline<void, BunbunStateShellAttack*> changeShellHook = hk::hook::trampoline([](BunbunStateShellAttack* state) {
    if (al::isFirstStep(state))
        al::tryOnStageSwitch(state->mBunbun, "SwitchChangeShellOn");

    changeShellHook.orig(state);
});

HkTrampoline<void, BunbunStateShellAttack*> recoverShellHook = hk::hook::trampoline([](BunbunStateShellAttack* state) {
    if (al::isFirstStep(state))
        al::tryOnStageSwitch(state->mBunbun, "SwitchRecoverShellOn");

    recoverShellHook.orig(state);
});

void pe::initBunbunModHooks()
{
    changeShellHook.installAtOffset(hk::ro::getMainModule(), 0x0000e7b0);
    recoverShellHook.installAtOffset(hk::ro::getMainModule(), 0x0000ef80);
    // BunbunStateShellAttackSpinHook::InstallAtOffset(0x0000ea80);
    // BunbunStateSpinAttackAttackHook::InstallAtOffset(0x0000f7e0);
}