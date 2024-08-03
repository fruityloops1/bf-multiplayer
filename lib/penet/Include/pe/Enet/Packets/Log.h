#pragma once

#include "pe/Enet/IPacket.h"

namespace pe {
namespace enet {

    class ToS_Log : public IPacket {
        const char* mData = nullptr;
        u8 mLogType = 0;

    public:
        ToS_Log() { }
        ToS_Log(const char* msg, u8 logType)
            : mData(msg)
            , mLogType(logType)
        {
        }

        ~ToS_Log() override;
        size_t calcSize() const override;
        void build(void* outData) const override;
        void read(const void* data, size_t len) override;

        const char* getMsg() const { return mData; }
        u8 getType() const { return mLogType; }
    };

} // namespace enet
} // namespace pe
