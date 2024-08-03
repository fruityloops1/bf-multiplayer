#pragma once

#include "Game/System/GameDataHolderAccessor.h"

struct ScenarioInfo {
    int mIslandID;
    int mScenarioID;
};

class SingleModeDataFunction {
public:
    static void completeScenario(GameDataHolderWriter, const ScenarioInfo&);
    static void setGoalItemCheckpointPass(GameDataHolderWriter, int, int);
    static void setUnlockedPhase(GameDataHolderWriter, int phase);
    static void setUnlockedChar(GameDataHolderWriter, int character);
    static void setPhaseEnd(GameDataHolderWriter, bool isEnd);
    static void collectShard(GameDataHolderWriter, int zoneID, int shardID);

    static int getUnlockedPhase(GameDataHolderAccessor);
    static bool isShardCollected(GameDataHolderAccessor, int zoneID, int shardID);
};
