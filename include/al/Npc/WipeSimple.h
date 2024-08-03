#pragma once

#include "al/Layout/LayoutActor.h"

namespace al {

class WipeSimple : public LayoutActor {
    void* _124 = nullptr;

public:
    WipeSimple(const char* name, const char* archiveName, const LayoutInitInfo& info, const char* suffix = nullptr);

    void startOpen(int);
    void startClose(int);

    bool isCloseEnd() const;
};

} // namespace al
