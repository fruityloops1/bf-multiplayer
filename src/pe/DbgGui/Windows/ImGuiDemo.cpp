#include "pe/DbgGui/Windows/ImGuiDemo.h"
#include "heap/seadHeapMgr.h"
#include "imgui.h"
#include <sead/heap/seadHeap.h>

namespace pe {
namespace gui {

    void ImGuiDemo::update()
    {
    }

    void ImGuiDemo::draw()
    {
        if (getDbgGuiSharedData().showDemoWindow)
            ImGui::ShowDemoWindow(&getDbgGuiSharedData().showDemoWindow);
    }

} // namespace gui
} // namespace pe
