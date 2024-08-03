#pragma once

#include "Game/Layout/SingleModeSceneLayout.h"
#include "al/Nerve/NerveExecutor.h"
#include "prim/seadStringUtil.h"
#include <cstdio>

namespace pe {

class MultiplayerStatusText : public al::NerveExecutor {
    SingleModeSceneLayout* mLayout = nullptr;
    const char16_t* mTexts[3] { nullptr };

    static constexpr char sPaneName[] = "TxtMultiplayerStatus";
    static constexpr int sAnimFrames = 15;

    void updatePane();

public:
    MultiplayerStatusText(SingleModeSceneLayout* layout);

    SingleModeSceneLayout* getLayout() const { return mLayout; }

    void exeWait();

    void log(const char16_t* msg);

    template <typename... Args>
    void log(const char* fmt, Args... args)
    {
        int size = snprintf(nullptr, 0, fmt, args...);
        char* buf = (char*)__builtin_alloca(size + 1);
        snprintf(buf, size + 1, fmt, args...);
        size_t wideSize = (size + 1) * sizeof(char16_t);
        char16_t* wideBuf = (char16_t*)__builtin_alloca(wideSize);
        sead::StringUtil::convertUtf8ToUtf16(wideBuf, wideSize, buf, size);
        log(wideBuf);
    }
};

} // namespace pe
