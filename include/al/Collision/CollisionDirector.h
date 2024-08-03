#pragma once

#include "al/Execute/ExecuteDirector.h"

namespace al {

class CollisionDirector : public IUseExecutor {
public:
    CollisionDirector(ExecuteDirector*, int);
};

class IUseCollision {
    virtual CollisionDirector* getCollisionDirector() const = 0;
};

} // namespace al