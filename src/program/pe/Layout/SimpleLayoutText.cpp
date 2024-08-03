#include "pe/Layout/SimpleLayoutText.h"
#include "al/Nerve/NerveFunction.h"

namespace pe {

namespace {
    NERVE_DEF(SimpleLayoutText, Appear);
    NERVE_DEF(SimpleLayoutText, Wait);
    NERVE_DEF(SimpleLayoutText, End);
}

SimpleLayoutText::SimpleLayoutText(const al::LayoutInitInfo& info, const char* name, const char* archiveName)
    : LayoutActor(name)
{
    al::initLayoutActor(this, info, archiveName, nullptr);
    initNerve(&nrvSimpleLayoutTextAppear);
}

void SimpleLayoutText::appear()
{
    LayoutActor::appear();
    al::setNerve(this, &nrvSimpleLayoutTextAppear);
    al::startAction(this, "Appear");
}

void SimpleLayoutText::end()
{
    if (!al::isNerve(this, &nrvSimpleLayoutTextEnd))
        al::setNerve(this, &nrvSimpleLayoutTextEnd);
}

void SimpleLayoutText::exeAppear()
{
    if (al::isActionEnd(this))
        al::setNerve(this, &nrvSimpleLayoutTextWait);
}

void SimpleLayoutText::exeWait()
{
    if (al::isFirstStep(this))
        al::startAction(this, "Wait");
}

void SimpleLayoutText::exeEnd()
{
    if (al::isFirstStep(this))
        al::startAction(this, "End");
    if (al::isActionEnd(this))
        kill();
}

} // namespace pe