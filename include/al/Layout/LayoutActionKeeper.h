#pragma once

#include "al/Util/IUseName.h"

namespace al {

class LayoutActionKeeper {
};

class IUseLayoutAction : IUseName {
public:
    virtual LayoutActionKeeper* getLayoutActionKeeper() const = 0;
};

} // namespace al