#pragma once

#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveFunction.h"
#include "util/modules.hpp"
#include <cstdint>

class ProductSequence;

namespace pe {
namespace util {

    const al::Nerve* getNerveAt(uintptr_t offset);

    const void setNerveAt(al::IUseNerve* to, uintptr_t offset);

    bool isSequenceSingleModeNrv(const ProductSequence* sequence);
    bool isSequenceSingleModeGameOnlyNrv(const ProductSequence* sequence);

} // namespace util
} // namespace pe