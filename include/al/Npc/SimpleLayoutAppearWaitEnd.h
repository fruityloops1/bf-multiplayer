#pragma once

#include "al/Layout/LayoutActor.h"

namespace al {

class SimpleLayoutAppearWaitEnd : public al::LayoutActor {

public:
    SimpleLayoutAppearWaitEnd(const char* name, const char* layoutName, const LayoutInitInfo& info, const char* suffix, bool isLocalized);

    void appear() override;
    void startWait();
    void end();

    bool isWait() const;
    bool isEnd() const;

    void exeAppear();
    void exeWait();
    void exeEnd();
};

} // namespace al
