#include "pe/DbgGui/Windows/Hacks.h"
#include "al/Base/String.h"
#include "al/Draw/GraphicsStressDirector.h"
#include "al/Draw/GraphicsSystemInfo.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Nerve/Nerve.h"
#include "al/Scene/LiveActorKit.h"
#include "helpers/fsHelper.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "imgui.h"
#include "pe/Util/Nerve.h"

namespace al {

class DepthOfFieldDrawer;
class LightIntensityDirector;
class GraphicsAreaDirector;

} // namespace al

namespace pe {
namespace gui {

    static bool sIsDisabledStartClipped = false;
    static bool sIsDisabledBloom = false;
    static bool sIsDisabledTransparentWall = false;
    static bool sIsDisabledDynamicResolution = false;
    static bool sIsDisabledDepthOfField = false;
    static bool sIsDisabledLayouts = false;
    static bool sIsDisabledBgm = false;

    HkTrampoline<void, void*, const char*, int, int, int, int> startBgmHook = hk::hook::trampoline([](void* thisPtr, const char* bgm, int a, int b, int c, int d) -> void {
        if (sIsDisabledBgm && !al::isEqualString(bgm, "SingleModeTitle"))
            return;
        startBgmHook.orig(thisPtr, bgm, a, b, c, d);
    });
    HkTrampoline<void, void*, const char**> startBgm2Hook = hk::hook::trampoline([](void* thisPtr, const char** bgm) -> void {
        if (sIsDisabledBgm && !al::isEqualString(*bgm, "SingleModeTitle"))
            return;
        startBgm2Hook.orig(thisPtr, bgm);
    });
    HkTrampoline<void, void*, void*, const char**, bool> startBgm3Hook = hk::hook::trampoline([](void* thisPtr, void* a, const char** b, bool c) -> void {
        if (sIsDisabledBgm && !al::isEqualString(*b, "SingleModeTitle"))
            return;
        startBgm3Hook.orig(thisPtr, a, b, c);
    });

    HkTrampoline<void, al::LiveActor*> disableStartClipped = hk::hook::trampoline([](al::LiveActor* actor) {
        if (!sIsDisabledStartClipped)
            disableStartClipped.orig(actor);
    });

    HkTrampoline<void, al::LiveActor*> disableStartClippedByLod = hk::hook::trampoline([](al::LiveActor* actor) {
        if (!sIsDisabledStartClipped)
            disableStartClippedByLod.orig(actor);
    });

    HkTrampoline<void, al::LightIntensityDirector*, al::GraphicsAreaDirector*, const char*> disableBloom = hk::hook::trampoline([](al::LightIntensityDirector* lightDirector, al::GraphicsAreaDirector* graphicsDirector, const char* str) {
        if (!sIsDisabledBloom)
            disableBloom.orig(lightDirector, graphicsDirector, str);
    });

    static void disableTransparentWallHook(al::LiveActor* actor)
    {
        if (sIsDisabledTransparentWall)
            actor->kill();
        else
            al::trySyncStageSwitchAppearAndKill(actor);
    }

    HkTrampoline<void, void*> hideLayoutsHook = hk::hook::trampoline([](void* thisPtr) {
        if (!sIsDisabledLayouts)
            hideLayoutsHook.orig(thisPtr);
    });

    static void disableDynamicResolutionHook(al::GraphicsStressDirector* graphics)
    {
        static class : public al::Nerve {
            void execute(al::NerveKeeper*) const override { }
            void executeOnEnd(al::NerveKeeper*) const override { }
        } dummy;

        if (sIsDisabledDynamicResolution)
            graphics->initNerve(&dummy);
        else
            graphics->initNerve(pe::util::getNerveAt(0x013e7c30));
    }

    Hacks::Hacks()
    {
        disableStartClipped.installAtOffset(hk::ro::getMainModule(), 0x0085c3c0);
        disableStartClippedByLod.installAtOffset(hk::ro::getMainModule(), 0x0085c6f0);
        // DisableBloom::InstallAtOffset(0x0081f2c0);

        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x002d65b4, disableTransparentWallHook);
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0080196c, disableDynamicResolutionHook);
        hideLayoutsHook.installAtOffset(hk::ro::getMainModule(), 0x00971c80);
        startBgmHook.installAtOffset(hk::ro::getMainModule(), 0x008efaa0);
        startBgm2Hook.installAtOffset(hk::ro::getMainModule(), 0x008ecbc0);
        startBgm3Hook.installAtOffset(hk::ro::getMainModule(), 0x008eba40);
    }

    void Hacks::update() { }
    void Hacks::draw()
    {
        if (getDbgGuiSharedData().showHacks) {
            if (ImGui::Begin("Hacks", &getDbgGuiSharedData().showHacks)) {
                ImGui::Checkbox("Hide Layouts", &sIsDisabledLayouts);
                ImGui::Checkbox("Disable StartClipped (LAG)", &sIsDisabledStartClipped);
                if (ImGui::Checkbox("Disable Bloom", &sIsDisabledBloom))
                    hk::ro::getMainModule()->writeRo(0x009571d4, sIsDisabledBloom ? 0xD503201F : 0x97F92B97);
                if (ImGui::Checkbox("Disable DOF", &sIsDisabledDepthOfField)) {
                    hk::ro::getMainModule()->writeRo(0x0088ef40, sIsDisabledDepthOfField ? 0xD65F03C0 : 0x39420008);
                }

                ImGui::Separator();

                ImGui::Checkbox("Disable BGM", &sIsDisabledBgm);
                ImGui::Checkbox("Disable TransparentWalls", &sIsDisabledTransparentWall);
                ImGui::Checkbox("Disable Dynamic Resolution", &sIsDisabledDynamicResolution);

                ImGui::Separator();

                if (ImGui::Button("Copy Save File to SD Card")) {
                    FsHelper::LoadData arg;
                    arg.path = "save:/GameData.bin";
                    FsHelper::loadFileFromPath(arg);
                    FsHelper::writeFileToPath(arg.buffer, arg.bufSize, "sd:/BowsersFuryOnline/GameData.bin");
                    IM_FREE(arg.buffer);
                }
            }
            ImGui::End();
        }
    }

} // namespace gui
} // namepace pe
