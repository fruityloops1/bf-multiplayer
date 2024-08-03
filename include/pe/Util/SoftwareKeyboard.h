#pragma once

#include "al/Functor/FunctorV0M.h"
#include "al/Thread/AsyncFunctorThread.h"
#include "nn/swkbd/swkbd.h"
#include "pe/Util/Log.h"
#include <cstdlib>

namespace pe::util {

class SoftwareKeyboard : public al::AsyncFunctorThread {
public:
    using DoneCallback = void (*)(void* userData, bool closed, const nn::swkbd::String& str);

private:
    bool mThreadRunning = false;
    bool mIsEndThread = false;
    nn::os::Event mEvent;
    nn::swkbd::ShowKeyboardArg mKeyboardArg;
    u8* mWorkBuf = nullptr;
    u8* mTextCheckBuf = nullptr;
    DoneCallback mDoneCallback = nullptr;
    void* mCallbackUserData = nullptr;
    nn::swkbd::String mString;

    static constexpr size_t sBufsSize = 0x1000;

public:
    SoftwareKeyboard()
        : AsyncFunctorThread("KeyboardThread", al::FunctorV0M(this, &SoftwareKeyboard::threadFunc), 1)
    {
        mWorkBuf = (u8*)aligned_alloc(0x1000, nn::swkbd::GetRequiredWorkBufferSize(false));
        mTextCheckBuf = (u8*)aligned_alloc(0x1000, nn::swkbd::GetRequiredTextCheckWorkBufferSize());
        mString.strBuf = (char16*)aligned_alloc(0x1000, nn::swkbd::GetRequiredStringBufferSize());
        mString.bufSize = nn::swkbd::GetRequiredStringBufferSize();

        nn::os::InitializeEvent(&mEvent, false, nn::os::EventClearMode_AutoClear);
        start();
    }

    ~SoftwareKeyboard()
    {
        if (mThreadRunning) {
            mIsEndThread = true;
            nn::os::SignalEvent(&mEvent);
            while (mThreadRunning)
                nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(1));
        }

        free(mWorkBuf);
        free(mTextCheckBuf);
        free(mString.strBuf);
        nn::os::FinalizeEvent(&mEvent);
    }

    void threadFunc()
    {
        mThreadRunning = true;
        while (true) {
            nn::os::WaitEvent(&mEvent);

            if (mIsEndThread)
                break;

            bool closed = nn::swkbd::ShowKeyboard(&mString, mKeyboardArg) == 671;
            if (mDoneCallback)
                mDoneCallback(mCallbackUserData, closed, mString);
        }
        mThreadRunning = false;
    }

    void show(nn::swkbd::ShowKeyboardArg& arg, const char* headerText, const char* subText, const char* guideText, const char* okText, DoneCallback callback = nullptr, void* userData = nullptr, const char* initialText = "")
    {
        mCallbackUserData = userData;
        mDoneCallback = callback;
        mKeyboardArg = arg;

        mKeyboardArg.workBufSize = nn::swkbd::GetRequiredWorkBufferSize(false);
        mKeyboardArg.textCheckWorkBufSize = nn::swkbd::GetRequiredTextCheckWorkBufferSize();
        mKeyboardArg.workBuf = mWorkBuf;
        mKeyboardArg.textCheckWorkBuf = mTextCheckBuf;

        nn::swkbd::SetHeaderTextUtf8(&mKeyboardArg.keyboardConfig, headerText);
        nn::swkbd::SetSubTextUtf8(&mKeyboardArg.keyboardConfig, subText);
        nn::swkbd::SetOkTextUtf8(&mKeyboardArg.keyboardConfig, okText);
        nn::swkbd::SetGuideTextUtf8(&mKeyboardArg.keyboardConfig, guideText);
        nn::swkbd::SetInitialTextUtf8(&mKeyboardArg, initialText);
        nn::os::SignalEvent(&mEvent);
    }
};

} // namespace pe::util
