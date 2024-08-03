#pragma once

#include <sead/basis/seadTypes.h>

class SingleModeData {
public:
    u8 _0[0x80];
    u32 mBossDefeatedFlag;
    u32 _84;
    class IslandSaveDataHolder* _88;
    class IslandSaveDataHolder* _90;
    u8 _98[0x2C];
    int mNumGoalItems;
    int mUnlockedPhase;
    int _CC;
    bool _D0;
    bool mIsNewToPhase1;
    bool mIsNewToPhase2;
    bool mIsNewToPhase3;
    bool mIsNewToPhase4;
    bool mIsNewToPhase2Boss;
    bool mIsNewToPhase3Boss;
    bool mIsNewToPhase1BowserIntro;
    bool mIsNewToPhase2BowserIntro;
    bool mIsNewToPhase3BowserIntro;
    bool mIsNewToPhase1BowserExit;
    bool mIsNewToPhase2BowserExit;
    bool mIsNewToPhase3BowserExit;
    u8 _DD[0x17];
    int mPhase1DarkBowserHitPoint;
    int mPhase2DarkBowserHitPoint;
    int mPhase3DarkBowserHitPoint;
    int mPhase4DarkBowserHitPoint;
    int mPhase4DarkBowserHitPointFinal;
    int mPhase3DarkBowserHitPointPreBattle;
    int mPhase4DarkBowserHitPointPreBattle;
    u8 _110[0x38];
    bool mIsPhaseEnd;
};
