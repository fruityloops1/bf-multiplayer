#include "pe/ServerSaveData.h"
#include "nlohmann/json.hpp"
#include "pe/Enet/Console.h"
#include "pe/Util.h"
#include <filesystem>

namespace pe {

constexpr static const char sSaveDataFile[] = "SaveData.json";

void ServerSaveData::save()
{
    nlohmann::json data;
    data["jumpFlipState"] = jumpFlipState;
    data["numCollectedGoalItems"] = numCollectedGoalItems;

    nlohmann::json goalItems = nlohmann::json::array();
    for (const GoalItemCollectData& goalItem : collectedGoalItems) {
        nlohmann::json hash;
        hash["islandID"] = goalItem.islandID;
        hash["scenarioID"] = goalItem.scenarioID;
        goalItems.push_back(hash);
    }
    data["goalItems"] = goalItems;

    nlohmann::json islands = nlohmann::json::array();
    for (const std::pair<std::string, int>& island : islandToScenarioMap) {
        nlohmann::json hash;
        hash["name"] = island.first;
        hash["scenarioID"] = island.second;
        islands.push_back(hash);
    }

    data["islands"] = islands;

    nlohmann::json toads = nlohmann::json::array();
    for (int toad : unlockedToads) {
        islands.push_back(toad);
    }
    data["unlockedToads"] = toads;

    nlohmann::json shards = nlohmann::json::array();
    for (const GoalItemCollectData& shard : collectedShards) {
        nlohmann::json hash;
        hash["zoneID"] = shard.islandID;
        hash["shardID"] = shard.scenarioID;
        shards.push_back(hash);
    }
    data["collectedShards"] = shards;

    writeStringToFile(sSaveDataFile, data.dump());
    enet::Console::log("Wrote save file to %s", sSaveDataFile);

    enet::Console::log("Saving shared save to GameData.bin");
    writeBytesToFile("GameData.bin", this->sharedSave, this->sharedSaveSize);
}

void ServerSaveData::loadSharedDefault()
{
    uint8_t* sharedSaveData = readBytesFromFile("GameDataDefault.bin", &this->sharedSaveSize);
    memcpy(this->sharedSave, sharedSaveData, this->sharedSaveSize);
    free(sharedSaveData);
}

void ServerSaveData::load()
{
    if (!std::filesystem::exists(sSaveDataFile)) {
        enet::Console::log("No save file exists. Creating new save file.");
        enet::Console::log("No shared save file exists. Loading default.");
        loadSharedDefault();
        return;
    }
    std::string file = readStringFromFile(sSaveDataFile);
    nlohmann::json data;
    try {
        data = nlohmann::json::parse(file);
    } catch (const nlohmann::json::parse_error& e) {
        enet::Console::log("Save file parsing failed. Creating new save file.");
        return;
    }

    reset();

    jumpFlipState = data["jumpFlipState"];
    numCollectedGoalItems = data["numCollectedGoalItems"];

    nlohmann::json goalItems = data["goalItems"];
    for (const nlohmann::json& goalItem : goalItems) {
        GoalItemCollectData data;
        data.islandID = goalItem["islandID"];
        data.scenarioID = goalItem["scenarioID"];
        collectedGoalItems.push_back(data);
    }

    nlohmann::json islands = data["islands"];
    for (const nlohmann::json& island : islands) {
        islandToScenarioMap[island["name"]] = island["scenarioID"];
    }

    nlohmann::json toads = data["unlockedToads"];
    for (int toad : toads) {
        unlockedToads.push_back(toad);
    }

    nlohmann::json shards = data["collectedShards"];
    for (const nlohmann::json& shard : shards) {
        GoalItemCollectData data;
        data.islandID = shard["zoneID"];
        data.scenarioID = shard["shardID"];
        collectedShards.push_back(data);
    }

    uint8_t* sharedSaveData = readBytesFromFile("GameData.bin", &this->sharedSaveSize);
    memcpy(this->sharedSave, sharedSaveData, this->sharedSaveSize);
    free(sharedSaveData);

    enet::Console::log("Successfully loaded save file from %s", sSaveDataFile);
}

void ServerSaveData::reset()
{
    jumpFlipState = false;
    numCollectedGoalItems = 0;
    collectedGoalItems.clear();
    islandToScenarioMap.clear();
    unlockedToads.clear();
    collectedShards.clear();
    loadSharedDefault();
}

} // namespace pe
