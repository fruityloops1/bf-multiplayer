#pragma once

#include "Game/System/SaveDataAccessSequence.h"
#include "Game/System/SingleModeData.h"
#include "al/System/GameDataHolderBase.h"

class GameDataHolder : public al::GameDataHolderBase {
public:
    u8 _0[0x30];
    SingleModeData* mData = nullptr;
    u8 _38[0x38];
    SaveDataAccessSequence* mSaveAccessSequence;

    void setSingleModePlayingFileID(int id, bool);
    void readFromSaveDataBuffer();
};
