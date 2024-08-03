#include "Game/MapObj/JumpFlipPanel.h"
#include "al/Nerve/NerveFunction.h"
#include "pe/Util/Nerve.h"

bool JumpFlipPanel::isFlipping()
{
    return al::isNerve(this, pe::util::getNerveAt(0x013341d8 /* Flip */));
}
