#pragma once

#include "al/Nerve/NerveStateBase.h"
#include <sead/container/seadPtrArray.h>

namespace al {

class NerveStateCtrl {
public:
    struct State {
        al::NerveStateBase* mNerveStateBase;
        const al::Nerve* mNerve;
        const char* mName;
    };

private:
    sead::PtrArray<State> mStates;
    State* mCurrentState;

public:
    NerveStateCtrl(int bufferSize);

    void addState(al::NerveStateBase* state, const al::Nerve* nerve, const char* name);
    bool updateCurrentState();
    void startState(const al::Nerve* nerve);
    void update();

    State* findStateInfo(const al::Nerve* nerve);
    bool isCurrentStateEnd() const;
    void tryEndCurrentState();
};

} // namespace al