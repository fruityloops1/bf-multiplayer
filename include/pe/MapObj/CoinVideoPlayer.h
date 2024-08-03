#pragma once

#include "Game/MapObj/Coin.h"
#include "al/LiveActor/LiveActor.h"
#include <nn/os.h>

namespace pe {

extern bool badAppleFrames[6573][32][32];

class CoinVideoPlayer : public al::LiveActor {
public:
    constexpr static int sWidth = 32, sHeight = 32;

private:
    Coin* mCoins[sWidth][sHeight] { { nullptr } };
    int mFrame = 0;
    nn::os::Tick mStartTick = 0;

public:
    CoinVideoPlayer(const char* name)
        : LiveActor(name)
    {
    }

    void init(const al::ActorInitInfo& info) override;

    void exeWaitStart();
    void exePlay();
};

} // namespace pe