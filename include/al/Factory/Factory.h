#pragma once

#include "types.h"

namespace al {

template <typename Creator>
struct NameToCreator {
    const char* name;
    Creator creator;
};

template <typename T>
class Factory {
public:
    inline Factory(const char* factory_name)
        : mName(factory_name)
        , mEntries(nullptr)
        , mEntryAmount(0)
    {
    }
    template <int N>
    inline Factory(const char* factory_name, al::NameToCreator<T> (&entries)[N])
        : mName(factory_name)
    {
        initFactory(entries);
    }
    template <int N>
    inline void initFactory(al::NameToCreator<T> (&entries)[N])
    {
        mEntries = entries;
        mEntryAmount = N;
    }

    virtual const char* convertName(const char*) const;

private:
    const char* mName = nullptr;
    al::NameToCreator<T>* mEntries = nullptr;
    int mEntryAmount = 0;
    u8 unk[0x14]; // ByamlIter at the end
};

} // namespace al