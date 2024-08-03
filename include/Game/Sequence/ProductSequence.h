#pragma once

#include "Game/System/GameDataHolder.h"
#include "al/Sequence/Sequence.h"

namespace pe {
class ProductStateTest;
} // namespace pe

class IUseUnknownSequence {
    virtual void unknown(const char*);
};

class ProductSequence : public al::Sequence, IUseUnknownSequence {
    void* unk1;
    GameDataHolder* mGameDataHolder;
    void* unk2[3];

public:
    class ProductStateBoot* mStateBoot;
    class ProductStateTitle* mStateTitle;
    class ProductStateSingleModeOpening* mStateSingleModeOpening;
    class ProductStateSingleMode* mStateSingleMode;
    class ProductStateSingleModeEnding* mStateSingleModeEnding;
    class ProductStateCourseSelect* mStateCourseSelect;
    class ProductStateNetworkStop* mStateNetworkStop;
    class ProductStateStage* mStateStage;
    class ProductStateEnding* mStateEnding;
    class ProductStateAfterEndingEvent* mStateAfterEndingEvent;
    class ProductStateTopMenu* mStateTopMenu;

    class ProductStateLuigiBros* mStateLuigiBros;
    u8 _140[0x70];
    class pe::ProductStateTest* mStateTest; // fake

    ~ProductSequence() override;
    void init(const al::SequenceInitInfo& info) override;
    void update() override;
    void drawMain() override;
    void drawSub() override;
    bool isDisposable() const override;
    al::Scene* getCurrentScene() const override;
    void unknown(const char*) override;

    void exeTitle();
};
