#include "pe/DbgGui/MenuBar.h"
#include "imgui.h"
#include "lib/util/modules.hpp"
#include "pe/Enet/Enet.h"
#include "pe/Enet/NetClient.h"
#include "pe/Hacks/RCSPlayers.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/Version.h"

namespace pe {
namespace gui {

    void MenuBar::update()
    {
    }

    void MenuBar::draw()
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Windows")) {
                ImGui::Checkbox("Demo Window", &getDbgGuiSharedData().showDemoWindow);
                ImGui::Checkbox("Log", &getDbgGuiSharedData().showLog);
                ImGui::Checkbox("HeapViewer", &getDbgGuiSharedData().showHeapViewer);
                ImGui::Checkbox("ActorBrowser", &getDbgGuiSharedData().showActorBrowser);
                ImGui::Checkbox("Hacks", &getDbgGuiSharedData().showHacks);
                ImGui::Checkbox("RCSCamera", &getDbgGuiSharedData().showRCSCamera);
                ImGui::Checkbox("Multiplayer", &getDbgGuiSharedData().showMultiplayer);
                ImGui::Checkbox("PoseViewer", &getDbgGuiSharedData().showPoseViewer);
                ImGui::EndMenu();
            }
            ImGui::Separator();
            printBuildIdImgui();
            ImGui::Separator();
            if (ImGui::Button("Connect")) {
            }
            ImGui::Separator();

            char buf[256];
            enet::getNetClient()->printStatusMsg(buf);
            ImGui::Text("%s", buf);

            ImGui::Separator();

            ImGui::SetNextItemWidth(100.f);
            static constexpr const char* items[] {
                "Mario", "Luigi", "Peach", "Kinopio", "Rosetta"
            };
            ImGui::Combo("##PlayerType", reinterpret_cast<int*>(&RCSPlayers::getCurrentPlayerType()), items, 5);

            ImGui::EndMainMenuBar();
        }
    }

} // namespace gui
} // namespace pe
