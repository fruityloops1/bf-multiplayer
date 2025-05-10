#include "pe/Multiplayer/MultiplayerStatusText.h"
#include "Game/Layout/CounterGoalItemParts.h"
#include "al/Layout/LayoutActor.h"
#include "al/Nerve/NerveFunction.h"
#include "heap/seadHeapMgr.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Util/Math.h"
#include <string>

namespace pe {

namespace {
    NERVE_DEF(MultiplayerStatusText, Wait)
}

MultiplayerStatusText::MultiplayerStatusText(SingleModeSceneLayout* layout)
    : NerveExecutor("MultiplayerStatusText")
    , mLayout(layout)
{
    initNerve(&nrvMultiplayerStatusTextWait);
}

void MultiplayerStatusText::updatePane()
{
    char16_t buffer[1024] { 0 };
    int cursor = 0;
    for (int i = 0; i < 3; i++) {
        const char16_t* text = mTexts[i];
        if (text) {
            size_t size = std::char_traits<char16_t>::length(text);
            std::char_traits<char16_t>::copy(&buffer[cursor], text, size);
            cursor += size;
            buffer[cursor] = '\n';
            cursor++;
        }
    }
    buffer[cursor] = '\0';

    al::setPaneString(mLayout, sPaneName, buffer);
}

void MultiplayerStatusText::exeWait()
{
    if (al::isFirstStep(this))
        updatePane();

    if (al::isStep(this, 360))
        log(nullptr);
}

void MultiplayerStatusText::log(const char16_t* msg)
{
    sead::ScopedCurrentHeapSetter setter(pe::gui::getPeepaHeap());

    if (mTexts[0]) {
        delete mTexts[0];
        mTexts[0] = nullptr;
    }

    char16_t* dup = nullptr;
    if (msg) {
        size_t size = std::char_traits<char16_t>::length(msg);
        dup = new char16_t[size + 1];
        std::char_traits<char16_t>::copy(dup, msg, size + 1);
    }

    mTexts[0] = mTexts[1];
    mTexts[1] = mTexts[2];
    mTexts[2] = dup;

    al::setNerve(this, &nrvMultiplayerStatusTextWait);
}

} // namespace pe
