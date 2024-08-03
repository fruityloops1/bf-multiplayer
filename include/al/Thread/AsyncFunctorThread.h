#pragma once

#include "al/Functor/FunctorBase.h"
#include "thread/seadDelegateThread.h"
#include <sead/thread/seadDelegateThread.h>

namespace al {

class AsyncFunctorThread {
    void* _0;
    sead::DelegateThread* mDelegateThread;
    FunctorBase* mFunctor;
    bool mIsDone;

public:
    AsyncFunctorThread(const sead::SafeString& name, const FunctorBase& functor, int block /* 0 = block, 1 = nonblock */, int stackSize = 0x1000);
    void start();

    bool isDone() const { return mIsDone; }
};

} // namespace al
