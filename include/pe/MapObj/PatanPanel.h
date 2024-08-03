#pragma once

#include "al/LiveActor/LiveActor.h"

namespace pe {

class PatanPanel : public al::LiveActor {
    PatanPanel* const mLast = nullptr;
    PatanPanel* mNext = nullptr;
    float mDegreesToRotate = 180;
    float mDegreesToRotateToNext = 180;

    sead::Vector3f mInitialTrans;
    sead::Quatf mInitialQuat;

    void rotateX(float degree, bool direction);
    PatanPanel* findStartPanel();
    constexpr static float sPanelWidth = 287.5f;

public:
    PatanPanel(const char* name)
        : al::LiveActor(name)
    {
    }
    PatanPanel(PatanPanel* last)
        : al::LiveActor("PatanPanel Child")
        , mLast(last)
    {
    }

    void init(const al::ActorInitInfo& info) override;

    void exeOpenStart();
    void exeOpen();
    void exeStartNext();
    void exeCloseSign();
    void exeCloseFace();
    void exeClose();

    void start();
    void startClose();
    bool isClosing() const;
};

} // namespace pe