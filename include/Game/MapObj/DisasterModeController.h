#pragma once

#include "Game/MapObj/SuperBowser.h"
#include "Game/MapObj/SuperBowserShell.h"
#include "al/Functor/FunctorBase.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Scene/SceneObjHolder.h"

class DisasterSpikeDirector;
class DisasterBlockDirector;
class GameSkyProjection;
class DemoCutscene;
class DemoAnimatic;
class DisasterLightning;
class SplatterPlotter;

class DisasterModeController : public al::LiveActor, public al::ISceneObj {
public:
    void* _150;
    DisasterSpikeDirector* mSpikeDirector;
    DisasterBlockDirector* mBlockDirector;
    void* _168;
    void* _170;
    GameSkyProjection* mSkyLakeProjection;
    GameSkyProjection* mSkyDisasterProjection;
    GameSkyProjection* mSkyDisasterHardProjection;
    GameSkyProjection* mSkySuperHardProjection;
    GameSkyProjection* mSkyFinalCutsceneProjection;
    void* _1a0;
    int mFramesOfProsperityTransition;
    int mFramesOfProsperity;
    int mFramesOfAnticipationTransition;
    int mFramesOfAnticipation;
    int mFramesOfDisasterTransition;
    int mFramesOfDisaster;
    int mFramesUntilDisaster;
    int mProgressFrames;
    u8 unk2[0x1C];
    SuperBowser* mSuperBowser = nullptr;
    SuperBowserShell* mSuperBowserShell = nullptr;
    u8 unk3[0x03];
    bool mDisableClock;
    u8 _1FC[0x2c];
    DemoCutscene* mDisasterModeStartCutscene;
    DemoAnimatic* mRetreatDemo;
    u8 _238[0x28];
    al::FunctorBase* mRainFunctor;
    int _268;
    int mPostBossPeaceTime;
    void* _270;
    sead::PtrArray<DisasterLightning> mDisasterLightningPool;
    u8 _288[0x50];
    int pDemoTransitionFrame;
    int pDemoDayTransitionFrame;
    int pDemoDisappearDayTransitionFrame;
    void* _2E4;
    SplatterPlotter* mSpikePlotter;
    u8 _2F8[0x18];
    bool mIsRaining;

    DisasterModeController(const char* name);

    static DisasterModeController* tryGetController(const al::IUseSceneObjHolder* user);

    void beginImmediate(bool);
    void endInstantly(bool, bool);
    void updateRainEffects();
};
