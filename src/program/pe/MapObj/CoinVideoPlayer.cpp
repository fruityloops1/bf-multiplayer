#include "pe/MapObj/CoinVideoPlayer.h"
#include "Game/MapObj/Coin.h"
#include "al/Bgm/BgmFunction.h"
#include "al/LiveActor/ActorClippingFunction.h"
#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveFunction.h"
#include <nn/os.h>

namespace pe {

namespace {
    NERVE_DEF(CoinVideoPlayer, WaitStart);
    NERVE_DEF(CoinVideoPlayer, Play);
}

void CoinVideoPlayer::init(const al::ActorInitInfo& info)
{
    al::initActorSceneInfo(this, info);
    al::initActorPoseTRSV(this);
    al::initActorSRT(this, info);
    al::initActorClipping(this, info);
    al::initExecutorEnemyMovement(this, info);
    al::initActorAudioKeeper(this, info, "CoinVideoPlayer", al::getTransPtr(this), &sead::Matrix34f::ident);
    al::invalidateClipping(this);
    al::initNerve(this, &nrvCoinVideoPlayerWaitStart);

    for (int y = 0; y < sHeight; y++)
        for (int x = 0; x < sWidth; x++) {
            Coin*& coin = mCoins[x][y];
            coin = new Coin("CoinVideoPlayer Coin");
            al::initCreateActorNoPlacementInfo(coin, info);

            al::copyPose(coin, this);
            sead::Vector3f* coinTrans = al::getTransPtr(coin);
            coinTrans->x += x * 100;
            coinTrans->y -= y * 100;
        }
    makeActorAppeared();
}

void CoinVideoPlayer::exeWaitStart()
{
    if (al::isFirstStep(this)) {
        al::startBgm(this, "Porter", 0, 0);
        mStartTick = nn::os::GetSystemTick();
        al::setNerve(this, &nrvCoinVideoPlayerPlay);
    }
}

void CoinVideoPlayer::exePlay()
{
    mFrame = ((s64)nn::os::GetSystemTick() - mStartTick) / (nn::os::GetSystemTickFrequency() / 30);
    if (mFrame >= 6573)
        mFrame = 0;

    for (int y = 0; y < sHeight; y++)
        for (int x = 0; x < sWidth; x++) {
            Coin* coin = mCoins[x][y];
            bool active = badAppleFrames[mFrame][y][x];
            if (active != !al::isDead(coin))
                active ? coin->appear() : coin->kill();
        }
}

} // namespace pe