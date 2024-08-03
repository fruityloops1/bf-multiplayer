#pragma once

#include "al/Debug/HioNode.h"
#include "al/Nerve/NerveKeeper.h"

namespace al {

class NerveExecutor : public IUseNerve, public HioNode {
    al::NerveKeeper* mNerveKeeper = nullptr;

public:
    NerveExecutor(const char* name);

    virtual NerveKeeper* getNerveKeeper() const;
    virtual ~NerveExecutor() = default;

    void initNerve(const Nerve*, int step = 0);
    void updateNerve();
};

} // namespace al