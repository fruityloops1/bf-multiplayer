#pragma once

#include "Game/Layout/CounterGoalItemParts.h"
#include "Game/Layout/ShardCounterParts.h"
#include "Game/System/GameDataHolder.h"
#include "al/Layout/LayoutActor.h"
#include "al/Player/PlayerHolder.h"
#include "al/Scene/ISceneObj.h"

class GreenStarKeeper;
class PlayerAliveWatcher;
class ProjectItemDirector;
class SingleModeSceneLayout : public al::LayoutActor, public al::ISceneObj {
    u8 _120[0x10];
    CounterGoalItemParts* mCounterGoalItem = nullptr;
    u8 _148[0x48];

public:
    ShardCounterParts* mShardCounterParts = nullptr;

    SingleModeSceneLayout(const al::LayoutInitInfo& info, GameDataHolder*, const char*, const GreenStarKeeper*, const al::PlayerHolder*, const PlayerAliveWatcher*, ProjectItemDirector*);

    CounterGoalItemParts* getCounterGoalItemParts() const { return mCounterGoalItem; }
    void addShard(int zoneID, int shardID, bool, bool);
};
