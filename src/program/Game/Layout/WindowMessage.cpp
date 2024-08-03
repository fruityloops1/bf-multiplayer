#include "Game/Layout/WindowMessage.h"
#include "al/Nerve/Nerve.h"
#include "pe/Util/Nerve.h"

namespace {

NERVE_DEF(WindowMessage, Appear);

}

void WindowMessage::appearWithSingleMessage(const char16_t* message)
{
    al::startAction(this, "Appear", "Main");
    LayoutActor::appear();
    mMessage = message;
    mPageAmount = 0;
    mPageIndex = 0;
    al::setPaneString(this, "TxtMessage", this->mMessage);
    al::setNerve(this, &nrvWindowMessageAppear);
}