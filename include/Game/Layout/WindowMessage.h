#pragma once

#include "al/Layout/LayoutActor.h"
#include "al/Layout/LayoutInitInfo.h"

class WindowMessage : public al::LayoutActor {
    void* unk;
    const char16_t* mMessage = nullptr;
    int unk1;
    int mPageAmount;
    int mPageIndex;
    int unk2;

public:
    WindowMessage(const al::LayoutInitInfo& info, const char* archiveName, const char* name, const char* = nullptr);

    void appearWithSystemMessage(const char* msbt, const char* message, int);
    void appearWithSingleMessage(const char16_t* message);

    void exeAppear();
    void exeWait();
};