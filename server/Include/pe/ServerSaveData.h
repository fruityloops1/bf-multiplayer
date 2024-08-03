#pragma once

#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace pe {

struct ServerSaveData {
    int numCollectedGoalItems = 0;

    struct GoalItemCollectData {
        int islandID;
        int scenarioID;
    };

    std::vector<GoalItemCollectData> collectedGoalItems;
    std::vector<GoalItemCollectData> collectedShards;
    std::map<std::string, int> islandToScenarioMap;
    std::vector<int> unlockedToads;
    bool jumpFlipState = false;
    size_t sharedSaveSize = 0;
    uint8_t sharedSave[(std::numeric_limits<uint16_t>::max)() + 0x100];

    bool isGoalItemAlreadyCollected(int islandID, int scenarioID) const
    {
        for (const GoalItemCollectData& shine : collectedGoalItems) {
            if (shine.islandID == islandID && shine.scenarioID == scenarioID)
                return true;
        }
        return false;
    }

    void save();
    void load();
    void reset();

    void loadSharedDefault();
};

} // namespace pe
