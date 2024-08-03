#pragma once

#include "al/Factory/Factory.h"
#include "al/LiveActor/LiveActor.h"

namespace al {

using CreateActorFunctionType = al::LiveActor* (*)(const char*);
using ActorFactoryTableEntry = NameToCreator<CreateActorFunctionType>;

class ActorFactory : public Factory<CreateActorFunctionType> {
public:
    ActorFactory(const char* name);
};

template <typename T>
LiveActor* createActorFunction(const char* name);

} // namespace al