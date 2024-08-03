#pragma once

#include "Game/Factory/ProjectActorFactory.h"
#include "al/LiveActor/LiveActor.h"

namespace pe {

class ProjectActorFactory : public ::ProjectActorFactory {
public:
    ProjectActorFactory();
};

template <typename T>
al::LiveActor* createActorFunction(const char* name)
{
    return new T(name);
}

void initProjectActorFactoryHook();

} // namespace pe