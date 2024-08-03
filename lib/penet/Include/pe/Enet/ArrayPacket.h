#pragma once

#include "pe/Enet/IPacket.h"
#include "pe/Enet/Impls.h"
#include <cstring>

namespace pe {
namespace enet {

    template <typename T, u32 MaxSize = 32>
    class ArrayPacket : public IPacket {
        u32 mNumEntries = 0;
        T* mEntries = nullptr;
        enum AllocSource : u8 {
            Ctor,
            New
        } mAllocSource
            = AllocSource::Ctor;

    public:
        ArrayPacket() { }

        ArrayPacket(u32 numEntries, T* entries)
            : mNumEntries(numEntries)
            , mEntries(entries)
        {
        }

        ArrayPacket(u32 numEntries)
            : mNumEntries(numEntries)
            , mEntries(new T[mNumEntries])
            , mAllocSource(AllocSource::New)
        {
        }

        ~ArrayPacket() override
        {
            if (mAllocSource == AllocSource::New)
                delete[] mEntries;
        }

        u32 size() const { return mNumEntries; }
        T* getEntries() const { return mEntries; }
        T* operator[](u32 idx)
        {
            if (idx >= mNumEntries) {
                PENET_ABORT("pe::enet::ArrayPacket::operator[]: idx >= mNumEntries (%u >= %u)", idx, mNumEntries);
            }

            return &mEntries[idx];
        }

        size_t calcSize() const override { return sizeof(u32) + sizeof(T) * mNumEntries; }

        void build(void* outData) const override
        {
            if (mEntries == nullptr)
                PENET_ABORT("pe::enet::ArrayPacket::build: mEntries == nullptr", 0);

            *reinterpret_cast<u32*>(outData) = mNumEntries;
            std::memcpy(reinterpret_cast<u8*>(outData) + sizeof(u32), mEntries, sizeof(T) * mNumEntries);
        }

        void read(const void* data, size_t len) override
        {
            mNumEntries = *reinterpret_cast<const u32*>(data);

            if (mNumEntries > MaxSize)
                PENET_ABORT("pe::enet::ArrayPacket::read: mNumEntries > MaxSize (%u)", MaxSize);

            mAllocSource = AllocSource::New;
            mEntries = new T[mNumEntries];
            std::memcpy(mEntries, reinterpret_cast<const u8*>(data) + sizeof(u32), sizeof(T) * mNumEntries);
        }
    };

} // namespace enet
} // namespace pe
