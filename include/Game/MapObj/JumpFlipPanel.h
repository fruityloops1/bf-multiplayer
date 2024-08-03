#pragma once

#include "al/LiveActor/LiveActor.h"

class JumpFlipPanel : public al::LiveActor {
    void* _144 = nullptr;
    u32 _14C;

public:
    float mRotateBoneZRotation;

private:
    bool mFlipState;
    u8 _15C[0x40];

private:
    void* _1A0 = nullptr;
    void* _1A8 = nullptr;

public:
    JumpFlipPanel(const char* name);
    void init(const al::ActorInitInfo& info) override;
    bool isFarLodSwitchOkay() override;

    void start();
    bool isFlipping();

    void exeWait();
    void exeSwitchOffStart();
    void exeFlip();
};
