#pragma once

#include "Game/MapObj/IslandHolder.h"
#include "container/seadPtrArray.h"

class IslandKeeper {
    u8 _0[0x28];
public:
    sead::PtrArray<IslandHolder> mIslands;
    sead::PtrArray<int> _38; // ?
    sead::PtrArray<int> _48; // ?

    IslandHolder* getIslandHolder(int islandID)
    {
        if (islandID < mIslands.size())
            return mIslands[islandID];
        return nullptr;
    }
};
