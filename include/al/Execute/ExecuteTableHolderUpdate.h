#pragma once

#include "al/Execute/ExecuteOrder.h"

namespace al {

class ExecuteSystemInitInfo;

class ExecuteTableHolderUpdate {
public:
    void init(const ExecuteSystemInitInfo& info, const ExecuteOrder* order, int size, bool something);
};

} // namespace al
