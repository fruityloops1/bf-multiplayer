#include "pe/Enet/Enet.h"
#include "enet/enet.h"
#include "hk/diag/diag.h"
#include "pe/Enet/NetClient.h"
#include "pe/Enet/ProjectPacketHandler.h"
#include "pe/Util/Log.h"
#include <sead/heap/seadExpHeap.h>

namespace pe {
namespace enet {

    static sead::ExpHeap* sEnetHeap = nullptr;

    static void* malloc(size_t size) { return sEnetHeap->alloc(size); }
    static void free(void* ptr) { return sEnetHeap->free(ptr); }
    static void no_memory() { HK_ABORT("no_memory", 0); }

    sead::ExpHeap* getEnetHeap() { return sEnetHeap; }

    void initialize(sead::Heap* parent)
    {
        sEnetHeap = sead::ExpHeap::create(1024 * 768, "EnetHeap", parent, 8, sead::ExpHeap::cHeapDirection_Forward, false);
        sead::ScopedCurrentHeapSetter setter(sEnetHeap);

        const ENetCallbacks callbacks { malloc, free, no_memory };
        if (enet_initialize_with_callbacks(ENET_VERSION, &callbacks) != 0)
            pe::err("ENet initialization failed");
    }

} // namespace enet
} // namespace pe
