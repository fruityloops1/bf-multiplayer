#pragma once

#include <cstddef>
#include <sead/heap/seadExpHeap.h>

namespace pe {
namespace enet {
    class NetClient;

    void initialize(sead::Heap* parent);
    sead::ExpHeap* getEnetHeap();

} // namespace enet
} // namespace pe
