#pragma once
#include <cstdint>

namespace exl::replace {

void ReplaceData(uintptr_t target, uintptr_t source, std::size_t size);

}