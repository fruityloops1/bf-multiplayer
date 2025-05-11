#pragma once

#include "hk/diag/diag.h"
#include "pe/Enet/NetClient.h"
#include "pe/Enet/Packets/Log.h"
#include <cstdio>
#include <sead/container/seadPtrArray.h>
#include <sead/heap/seadHeap.h>

namespace pe {

    enum class LogType : u8
    {
        Log,
        Warning,
        Error
    };

    void initializeLog(sead::Heap* parent);
    sead::PtrArray<LogType>& getLogLines();
    char* addLog(LogType type, size_t len);

#define PE_UTIL_LOG_TEMPLATE(NAME, TYPE)                  \
    template <typename... Args>                           \
    void NAME(const char* fmt, Args... args) {            \
        size_t size = snprintf(nullptr, 0, fmt, args...); \
        char* msg = addLog(LogType::TYPE, size);          \
        snprintf(msg, size + 1, fmt, args...);            \
        enet::NetClient* client = enet::getNetClient();   \
        if (client && client->isConnected()) {            \
            enet::ToS_Log packet(msg, (u8)LogType::TYPE); \
            client->sendPacket(&packet);                  \
            client->flush();                              \
        }                                                 \
        hk::diag::debugLog("%s", msg);                    \
    }

    PE_UTIL_LOG_TEMPLATE(log, Log);
    PE_UTIL_LOG_TEMPLATE(warn, Warning);
    PE_UTIL_LOG_TEMPLATE(err, Error);

#undef PE_UTIL_LOG_TEMPLATE

} // namespace pe
