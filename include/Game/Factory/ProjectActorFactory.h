#pragma once

#include "al/Factory/ActorFactory.h"

class ProjectActorFactory : public al::ActorFactory {

public:
    static al::ActorFactoryTableEntry sActorEntries[459];

    ProjectActorFactory()
        : ActorFactory("アクターファクトリー")
    {
        initFactory(sActorEntries);
    }
    const char* convertName(const char* name) const override;
};