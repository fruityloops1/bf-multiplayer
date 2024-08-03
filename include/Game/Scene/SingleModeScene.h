#pragma once

#include "Game/Layout/SingleModeSceneLayout.h"
#include "Game/MapObj/IslandKeeper.h"
#include "Game/MapObj/RaidonSurf.h"
#include "Game/Scene/InGameSceneBase.h"

class SingleModeScene : public InGameSceneBase {
public:
    void movement() override;

    u8 unk[0xdf];
    IslandKeeper* mIslandKeeper = nullptr;
    u8 _1C8[0xa8];
    RaidonSurf* mRaidonSurf = nullptr;
    u8 _278[0x50];
    SingleModeSceneLayout* mLayout = nullptr;

    virtual bool isScenarioComplete(int, int);
    virtual void createViewRenderer(/*al::GraphicsSystemInfo**/);
    virtual void deleteViewRenderer(/*al::ViewRenderers**/);
    virtual bool isChangePhase() const;
    virtual bool allowRestartPoint() const;
    virtual bool isIslandScene() const;
    virtual bool isBossScene() const;
    virtual bool isGameEnd() const;
    virtual bool isPhaseEnd() const;
};