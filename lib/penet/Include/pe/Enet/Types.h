#pragma once

#ifdef NNSDK
#include <nn/account.h>
#include <sead/basis/seadTypes.h>
#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>

#define PE_PRINT_UUID(UID, PRINTF)                                                     \
    {                                                                                  \
        union {                                                                        \
            ::nn::account::Uid uid;                                                    \
            u8 m_ByteStorage[0x10];                                                    \
        } storage { UID };                                                             \
        PRINTF("%.2x%.2x%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x", \
            storage.m_ByteStorage[0],                                                  \
            storage.m_ByteStorage[1],                                                  \
            storage.m_ByteStorage[2],                                                  \
            storage.m_ByteStorage[3],                                                  \
            storage.m_ByteStorage[4],                                                  \
            storage.m_ByteStorage[5],                                                  \
            storage.m_ByteStorage[6],                                                  \
            storage.m_ByteStorage[7],                                                  \
            storage.m_ByteStorage[8],                                                  \
            storage.m_ByteStorage[9],                                                  \
            storage.m_ByteStorage[10],                                                 \
            storage.m_ByteStorage[11],                                                 \
            storage.m_ByteStorage[12],                                                 \
            storage.m_ByteStorage[13],                                                 \
            storage.m_ByteStorage[14],                                                 \
            storage.m_ByteStorage[15]);                                                \
    }
#else
#include <cstddef>
#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using f32 = float;
using f64 = double;

using char16 = char16_t;
using size_t = std::size_t;

namespace sead {

template <typename T>
struct Vector3 {
    T x, y, z;
};

using Vector3f = Vector3<float>;

template <typename T>
struct Quat {
    T x, y, z, w;

    static const Quat unit;
};

using Quatf = Quat<float>;

template <>
const Quatf Quatf::unit;

} // namespace sead

namespace nn {
namespace account {

    using Nickname = char[33];
    class Uid {
    public:
        bool IsValid() const { return m_Storage[0] != 0 || m_Storage[1] != 0; }

        union {
            u64 m_Storage[2];
            u8 m_ByteStorage[0x10];
        };
    };

#define PE_PRINT_UUID(UID, PRINTF)                                                     \
    {                                                                                  \
        PRINTF("%.2x%.2x%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x", \
            UID.m_ByteStorage[0],                                                      \
            UID.m_ByteStorage[1],                                                      \
            UID.m_ByteStorage[2],                                                      \
            UID.m_ByteStorage[3],                                                      \
            UID.m_ByteStorage[4],                                                      \
            UID.m_ByteStorage[5],                                                      \
            UID.m_ByteStorage[6],                                                      \
            UID.m_ByteStorage[7],                                                      \
            UID.m_ByteStorage[8],                                                      \
            UID.m_ByteStorage[9],                                                      \
            UID.m_ByteStorage[10],                                                     \
            UID.m_ByteStorage[11],                                                     \
            UID.m_ByteStorage[12],                                                     \
            UID.m_ByteStorage[13],                                                     \
            UID.m_ByteStorage[14],                                                     \
            UID.m_ByteStorage[15]);                                                    \
    }

} // namespace account
} // namespace nn

#endif
