#pragma once

#include "nn/g3d/ResFile.h"
#include "prim/seadSafeString.h"
#include <sead/filedevice/seadArchiveFileDevice.h>
#include <sead/resource/seadArchiveRes.h>

namespace al {

class Resource {
    sead::ArchiveRes* mSeadResource = nullptr;
    sead::ArchiveFileDevice* mSzsDevice = nullptr;
    sead::BufferedSafeStringBase<char> mResourcePath;
    u8 _28[0x90];
    Resource* mPatchData = nullptr;
    nn::g3d::ResFile* mResGraphicsFile = nullptr;

public:
    Resource(const sead::SafeString& path);
    Resource(const sead::SafeString& path, sead::ArchiveRes*);

    void loadPatchData();

    bool isExistFile(const sead::SafeString& name) const;
    bool isExistByml(const char*) const;
    int getEntryNum(const sead::SafeString&) const;
    void getEntryName(sead::BufferedSafeString* out, const sead::SafeString&, u32);
    u64 getFileSize(const sead::SafeString& name) const;
    const u8* getFile(const sead::SafeString& name) const;
    const u8* getByml(const sead::SafeString& name) const;
    const u8* tryGetByml(const sead::SafeString& name) const;
    const u8* getKcl(const sead::SafeString& name) const;
    const u8* tryGetKcl(const sead::SafeString& name) const;
    const u8* getPa(const sead::SafeString& name) const;
    const u8* getOtherFile(const sead::SafeString& name, u32*) const;
    const char* getArchiveName() const;
    bool tryCreateResGraphicsFile(const sead::SafeString&, nn::g3d::ResFile*);

    nn::g3d::ResFile* getResGraphicsFile() const { return mResGraphicsFile; }
};

static_assert(sizeof(Resource) == 0xc8);

} // namespace al