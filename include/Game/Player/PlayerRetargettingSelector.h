#pragma once

#include "Game/Player/IUsePlayerRetargettingSelector.h"
#include "Game/Player/PlayerAnimFunc.h"
#include "al/Anim/SklAnimRetargettingInfo.h"
#include <sead/basis/seadTypes.h>
#include <sead/container/seadPtrArray.h>

class PlayerRetargettingSelector : public IUsePlayerRetargettingSelector {
    void* inherit;
    int _10;
    int _14;
    const char* _18;
    void* _20;
    void** _28;

public:
    struct {
        bool ik : 1;
        bool hairCtrl : 1;
        bool skirtDynamics : 1;
        bool tailJointController : 1;
        bool hairJointController : 1;
    } mModelFlags;
    int _34;
    void* _38;

    virtual al::SklAnimRetargettingInfo* createRetargettingInfo(const char*, EPlayerFigure);
    virtual al::SklAnimRetargettingInfo* createDemoRetargettingInfo(const char*, EPlayerFigure);
    void applyDefaultRetargettingInfo(al::LiveActor*, int) override;
    void applyCharaRetargettingInfo(al::LiveActor*, int) override;
    void applyFigureRetargettingInfo(al::LiveActor*, int) override;
    al::SklAnimRetargettingInfo* getDefaultRetargettingInfo(int) override;
    al::SklAnimRetargettingInfo* getCharaRetargettingInfo(int) override;
    al::SklAnimRetargettingInfo* getFigureRetargettingInfo(int) override;

    static const char* getFileName();
};