#include "pe/DbgGui/Windows/Hacks.h"
#include "al/Base/String.h"
#include "al/Draw/GraphicsStressDirector.h"
#include "al/Draw/GraphicsSystemInfo.h"
#include "al/Nerve/Nerve.h"
#include "al/Scene/LiveActorKit.h"
#include "helpers/fsHelper.h"
#include "hook/trampoline.hpp"
#include "imgui.h"
#include "patch/code_patcher.hpp"
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

    HOOK_DEFINE_TRAMPOLINE(DisableStartClipped) { static void Callback(al::LiveActor*); };
    HOOK_DEFINE_TRAMPOLINE(DisableStartClippedByLod) { static void Callback(al::LiveActor*); };
    HOOK_DEFINE_TRAMPOLINE(DisableBloom) { static void Callback(al::LightIntensityDirector * lightDirector, al::GraphicsAreaDirector * graphicsDirector, const char* str); };
    HOOK_DEFINE_TRAMPOLINE(HideLayouts) { static void Callback(void* thisPtr); };
    HOOK_DEFINE_TRAMPOLINE(StartBgm) { static void Callback(void* thisPtr, const char* bgm, int a, int b, int c, int d); };
    HOOK_DEFINE_TRAMPOLINE(StartBgm2) { static void Callback(void* thisPtr, const char** bgm); };
    HOOK_DEFINE_TRAMPOLINE(StartBgm3) { static void Callback(void* thisPtr, void* a, const char** b, bool c); };

    void StartBgm::Callback(void* thisPtr, const char* bgm, int a, int b, int c, int d)
    {
        if (sIsDisabledBgm && !al::isEqualString(bgm, "SingleModeTitle"))
            return;
        Orig(thisPtr, bgm, a, b, c, d);
    }

    void StartBgm2::Callback(void* thisPtr, const char** bgm)
    {
        if (sIsDisabledBgm && !al::isEqualString(*bgm, "SingleModeTitle"))
            return;
        Orig(thisPtr, bgm);
    }

    void StartBgm3::Callback(void* thisPtr, void* a, const char** b, bool c)
    {
        if (sIsDisabledBgm && !al::isEqualString(*b, "SingleModeTitle"))
            return;
        Orig(thisPtr, a, b, c);
    }

    void DisableStartClipped::Callback(al::LiveActor* actor)
    {
        if (!sIsDisabledStartClipped)
            Orig(actor);
    }

    void DisableStartClippedByLod::Callback(al::LiveActor* actor)
    {
        if (!sIsDisabledStartClipped)
            Orig(actor);
    }

    void DisableBloom::Callback(al::LightIntensityDirector* lightDirector, al::GraphicsAreaDirector* graphicsDirector, const char* str)
    {
        if (!sIsDisabledBloom)
            Orig(lightDirector, graphicsDirector, str);
    }

    static void disableTransparentWallHook(al::LiveActor* actor)
    {
        if (sIsDisabledTransparentWall)
            actor->kill();
        else
            al::trySyncStageSwitchAppearAndKill(actor);
    }

    void HideLayouts::Callback(void* thisPtr)
    {
        if (!sIsDisabledLayouts)
            Orig(thisPtr);
    }

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
        using Patcher = exl::patch::CodePatcher;

        DisableStartClipped::InstallAtOffset(0x0085c3c0);
        DisableStartClippedByLod::InstallAtOffset(0x0085c6f0);
        // DisableBloom::InstallAtOffset(0x0081f2c0);

        Patcher(0x002d65b4).BranchLinkInst((void*)disableTransparentWallHook);
        Patcher(0x0080196c).BranchLinkInst((void*)disableDynamicResolutionHook);
        HideLayouts::InstallAtOffset(0x00971c80);
        StartBgm::InstallAtOffset(0x008efaa0);
        StartBgm2::InstallAtOffset(0x008ecbc0);
        StartBgm3::InstallAtOffset(0x008eba40);
    }

    void Hacks::update() { }
    void Hacks::draw()
    {
        if (getDbgGuiSharedData().showHacks) {
            if (ImGui::Begin("Hacks", &getDbgGuiSharedData().showHacks)) {
                ImGui::Checkbox("Hide Layouts", &sIsDisabledLayouts);
                ImGui::Checkbox("Disable StartClipped (LAG)", &sIsDisabledStartClipped);
                if (ImGui::Checkbox("Disable Bloom", &sIsDisabledBloom)) {
                    if (sIsDisabledBloom)
                        exl::patch::CodePatcher(0x009571d4).Write(exl::armv8::inst::Nop());
                    else
                        exl::patch::CodePatcher(0x009571d4).WriteInst(0x97F92B97);
                }
                if (ImGui::Checkbox("Disable DOF", &sIsDisabledDepthOfField)) {
                    if (sIsDisabledDepthOfField)
                        exl::patch::CodePatcher(0x0088ef40).Write(exl::armv8::inst::Ret());
                    else
                        exl::patch::CodePatcher(0x0088ef40).WriteInst(0x39420008);
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
