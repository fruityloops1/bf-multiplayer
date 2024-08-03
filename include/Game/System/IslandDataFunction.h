#pragma once

#include "al/Message/MessageSystem.h"
#include "al/Scene/SceneObjHolder.h"

class IslandDataFunction {
public:
    static const char16* getIslandName(al::IUseSceneObjHolder* user, al::IUseMessageSystem* msgUser, int islandID);
    static const char16* getIslandScenarioName(al::IUseSceneObjHolder* user, al::IUseMessageSystem* msgUser, int islandID, int scenarioID);
};
