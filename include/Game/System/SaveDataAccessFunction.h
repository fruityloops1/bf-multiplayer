#pragma once

#include "Game/System/GameDataHolder.h"

class SaveDataAccessFunction {
public:
    static void startSaveDataInit(GameDataHolder* holder);
    static void startSaveDataRead(GameDataHolder* holder, bool);
    static void startSaveDataWrite(GameDataHolder* holder, bool, int, bool);
    static void startSaveDataWriteSync(GameDataHolder* holder, bool);
    static bool updateSaveDataAccess(GameDataHolder* holder, bool); // wtf is this garbage haha
};
