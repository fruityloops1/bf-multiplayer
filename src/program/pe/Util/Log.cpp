#include "pe/Util/Log.h"
#include "diag/assert.hpp"
#include "heap/seadHeapMgr.h"
#include <sead/heap/seadExpHeap.h>

namespace pe {

constexpr s32 sMaxLines = 8192;

static sead::ExpHeap* sLogHeap;
static sead::PtrArray<LogType> sLogLines;

void initializeLog(sead::Heap* parent)
{
    sLogHeap = sead::ExpHeap::create(1024 * 256, "LogHeap", parent, 8, sead::ExpHeap::cHeapDirection_Forward, false);
    EXL_ASSERT(sLogHeap != nullptr, "Log heap creation failed");
    sLogLines.allocBuffer(sMaxLines, sLogHeap);

    log("Log initialized");
}

sead::PtrArray<LogType>& getLogLines()
{
    return sLogLines;
}

char* addLog(LogType type, size_t len)
{
    LogType* data = reinterpret_cast<LogType*>(sLogHeap->alloc(len + 2));
    *data = type;
    while (sLogLines.size() >= sMaxLines or sLogHeap->getFreeSize() < len + 256)
        sLogHeap->free(sLogLines.popFront());
    sLogLines.pushBack(data);
    return reinterpret_cast<char*>(data) + 1;
}

} // namespace pe
