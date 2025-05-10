#include "pe/Util/BuildId.h"
#include "pe/Util/Version.h"

// this needs to be fast to compile
namespace ImGui {
#define IMGUI_API
#define IM_FMTARGS(FMT) __attribute__((format(printf, FMT, FMT + 1)))
IMGUI_API void Text(const char* fmt, ...) IM_FMTARGS(1); // formatted text
} // namespace ImGui

namespace pe {
void printBuildIdImgui()
{
#define STRINGIZE(x) #x
#define EXPAND(x) STRINGIZE(x)
    ImGui::Text("peepa v" PE_VERSION " build " EXPAND(BUILD_ID));
#undef STRINGIZE
#undef EXPAND
}
} // namespace pe
