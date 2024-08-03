#pragma once

#include "al/Layout/LayoutActor.h"

class RCSControlGuideBar : public al::LayoutActor {
public:
    enum class GuideBarMsgType {
        Confirm = 10
    };

    void setCharacter(const char* characterName);
    void appear() override;
    void appearWithMessage(GuideBarMsgType type, int);
    void hide();
    void end();
};
