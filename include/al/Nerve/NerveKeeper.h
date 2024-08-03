#pragma once

#include "types.h"

namespace al {

class Nerve;
class NerveStateCtrl;
class NerveActionCtrl;

class NerveKeeper;
class IUseNerve {
public:
    virtual NerveKeeper* getNerveKeeper() const = 0;
};

class NerveActionCtrl;
class NerveKeeper {
    IUseNerve* mHost = nullptr;

    const Nerve* mLastNerve = nullptr;
    const Nerve* mNerve = nullptr;
    s32 mStep = 0;
    s32 _1c;
    NerveStateCtrl* mNerveStateCtrl = nullptr;
    NerveActionCtrl* mNerveActionCtrl = nullptr;

public:
    NerveKeeper(void*, const Nerve*, int numNerveStates);

    void update();
    void setNerve(const Nerve*);
    const Nerve* getCurrentNerve();
    void initNerveAction(al::NerveActionCtrl*);
    NerveStateCtrl* getNerveStateCtrl() const { return mNerveStateCtrl; }
    NerveActionCtrl* getNerveActionCtrl() const { return mNerveActionCtrl; }

    IUseNerve* getHost() const { return mHost; }
};

} // namespace al