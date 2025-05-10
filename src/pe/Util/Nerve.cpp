#include "pe/Util/Nerve.h"
#include "Game/Sequence/ProductSequence.h"
#include "hk/ro/RoUtil.h"

namespace pe {
namespace util {

    const al::Nerve* getNerveAt(uintptr_t offset)
    {
        return (al::Nerve*)(hk::ro::getMainModule()->range().start() + offset);
    }

    const void setNerveAt(al::IUseNerve* to, uintptr_t offset)
    {
        al::setNerve(to, getNerveAt(offset));
    }

    bool isSequenceSingleModeGameOnlyNrv(const ProductSequence* sequence)
    {
        return al::isNerve(sequence, pe::util::getNerveAt(0x0138cf88) /* SingleMode */);
    }

    bool isSequenceSingleModeNrv(const ProductSequence* sequence)
    {
        return al::isNerve(sequence, pe::util::getNerveAt(0x0138cf80) /* SingleModeOpening */) or al::isNerve(sequence, pe::util::getNerveAt(0x0138cf88) /* SingleMode */) or al::isNerve(sequence, pe::util::getNerveAt(0x0138cf90) /* SingleModeEnding */);
    }

} // namespace util
} // namespace pe
