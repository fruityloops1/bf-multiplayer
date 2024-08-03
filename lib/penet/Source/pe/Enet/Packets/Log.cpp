#include "pe/Enet/Packets/Log.h"
#include "pe/Enet/Impls.h"
#include <cstring>

namespace pe {
namespace enet {

    ToS_Log::~ToS_Log()
    {
        if (mData)
            PENET_FREE((void*)mData);
    }

    size_t ToS_Log::calcSize() const
    {
        if (mData == nullptr)
            PENET_ABORT("pe::enet::Log::calcSize: mData is nullptr", 0);
        return std::strlen(mData) + 2;
    }

    void ToS_Log::build(void* outData) const
    {
        if (mData == nullptr)
            PENET_ABORT("pe::enet::Log::build: mData is nullptr", 0);

        *reinterpret_cast<u8*>(outData) = mLogType;
        std::strcpy(reinterpret_cast<char*>(outData) + 1, mData);
    }

    void ToS_Log::read(const void* data, size_t len)
    {
        const char* charData = reinterpret_cast<const char*>(data) + 1;
        size_t strLen = std::strlen(charData);
        char* newData = (char*)PENET_MALLOC(strLen + 1);
        std::strncpy(newData, charData, len);
        mData = newData;
        mLogType = *reinterpret_cast<const u8*>(data);
    }

} // namespace enet
} // namespace pe
