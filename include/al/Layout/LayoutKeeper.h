#pragma once

#include "al/Util/IUseName.h"

namespace al {

class LayoutKeeper {
};

class IUseLayout : IUseName {
public:
    virtual LayoutKeeper* getLayoutKeeper() const = 0;
};

} // namespace al