#pragma once

#include "al/Layout/LayoutActor.h"
#include "al/Layout/LayoutInitInfo.h"

enum class WindowConfirmType : int {
    Report = 0,
    Single = 1,
    Double = 2
};

class WindowConfirm : public al::LayoutActor {

public:
    WindowConfirm(WindowConfirmType type, const al::LayoutInitInfo& info, const char* = nullptr);

    void appearWithSystemMessage(const char* msbt, const char* message, int, const char* = nullptr);
};