#pragma once

#include "al/LiveActor/LiveActor.h"

class Shards : public al::LiveActor {
    u8 _144[0x60];

public:
    int mShardID;
    int mZoneID;
    bool mIsShardCollected = false;
    u8 _1B8[64];
    al::LiveActor* mEmptyShardModel = nullptr;

    // FAKE
    bool mCollectedByHook = false;

    Shards(const char* name, bool);
};

class ShardsWatcher {
public:
    bool isFinalShard() const;
};
