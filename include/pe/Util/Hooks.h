#pragma once

#include "al/LiveActor/LiveActor.h"
#include "pe/Util/Nerve.h"
#include "util/modules.hpp"

namespace pe {
namespace util {

    template <uintptr_t nerveOffset>
    void setNerveHook(al::IUseNerve* user, const al::Nerve* prevNrv)
    {
        const al::Nerve* nerve = util::getNerveAt(nerveOffset);
        al::setNerve(user, nerve);
    }

    template <uintptr_t nerveOffset, uintptr_t condNerveOffset>
    void setNerveHookIfNot(al::IUseNerve* user, const al::Nerve* prevNrv)
    {
        const al::Nerve* nerve = prevNrv == util::getNerveAt(condNerveOffset) ? prevNrv : util::getNerveAt(nerveOffset);
        al::setNerve(user, nerve);
    }

    template <typename F>
    uintptr_t getFuncPtrOffset(F ptr)
    {
        struct {
            F ptrType;
            uintptr_t offset;
        } conv { ptr };
        return conv.offset - exl::util::modules::GetTargetStart();
    }

} // namespace
} // namespace