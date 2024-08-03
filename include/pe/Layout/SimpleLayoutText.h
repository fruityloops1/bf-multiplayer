#pragma once

#include "al/Layout/LayoutActor.h"
#include "al/Layout/LayoutInitInfo.h"

namespace pe {

class SimpleLayoutText : public al::LayoutActor {

public:
    SimpleLayoutText(const al::LayoutInitInfo& info, const char* name, const char* archiveName);

    void appear() override;
    void end();

    void exeAppear();
    void exeWait();
    void exeEnd();
};

} // namespace pe