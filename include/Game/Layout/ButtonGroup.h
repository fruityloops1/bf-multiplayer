#pragma once

#include "Game/Layout/ButtonCursorParts.h"
#include "al/Layout/LayoutActor.h"
#include "al/Layout/LayoutInitInfo.h"

class ButtonGroup {
    ButtonCursorParts* mCursor = nullptr;
    u8 _8[0x248];

public:
    ButtonGroup(const al::LayoutInitInfo& info, al::LayoutActor* host, const char* archiveName, const char* suffix, bool = false);

    void decide(const char* button);
    bool isDecide(const char* button) const;
    bool isDecideAny() const;
    bool isDecideEnd(const char* button) const;
    bool isDecideEndAny() const;
    bool isSelect(const char* button) const;
    void select(const char* button);
    void setPort(int port);
    void hideCursor();
    void showCursor();
    void showCursorAppear();
    void update();
    void updateAndCursorDefault(int port);
    const char* getSelectedButtonName() const;

    void invalidate();
    void validate();
    void reset();

    al::LayoutActor* getButton(const char* button) const;
    ButtonCursorParts* getCursor() const { return mCursor; }
};
