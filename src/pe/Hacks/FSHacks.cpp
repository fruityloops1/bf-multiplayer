#include "pe/Hacks/FSHacks.h"
#include "al/Base/String.h"
#include "al/File/FileFunction.h"
#include "al/Memory/MemorySystem.h"
#include "flips/libbps.h"
#include "heap/seadExpHeap.h"
#include "heap/seadHeapMgr.h"
#include "helpers/fsHelper.h"
#include "hk/diag/diag.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "nn/fs.h"
#include "pe/Enet/NetClient.h"
#include "pe/Util/Log.h"
#include "prim/seadEndian.h"
#include "prim/seadSafeString.h"
#include "resource/seadArchiveRes.h"
#include "resource/seadSZSDecompressor.h"
#include "vapours/results/results_common.hpp"
#include <cmath>

static sead::ExpHeap* sPatchHeap = nullptr;

s32 decodeSZSNxAsm64_(void* dst, const void* src);

namespace pe {

    static bool sHookDisabled = false;

    HkTrampoline<nn::Result, nn::fs::FileHandle*, const char*, int> openFileHook = hk::hook::trampoline([](nn::fs::FileHandle* out, const char* filePath, int mode) -> nn::Result {
        if (sHookDisabled)
            return openFileHook.orig(out, filePath, mode);

        static bool sdCardMounted = false;
        if (!sdCardMounted) {
            nn::fs::MountSdCardForDebug("sd");
            sdCardMounted = true;
        }

        if (std::strncmp(filePath, "content:/", 9) == 0) {
            char path[1024] { 0 };
            strcat(path, "sd:/BowsersFuryOnline/cache/");
            strcat(path, filePath + 9);
            if (FsHelper::isFileExist(path))
                return openFileHook.orig(out, path, mode);
        }

        return openFileHook.orig(out, filePath, mode);
    });

    static sead::ArchiveRes* loadArchiveHook(const sead::SafeString& path) {
        if (
            FsHelper::isFileExist(al::StringTmp<256>("content:/%s.sarc", path.cstr()).cstr())
            or FsHelper::isFileExist(al::StringTmp<256>("sd:/BowsersFuryOnline/cache/%s.sarc", path.cstr()).cstr())) {
            return al::loadArchiveWithExt(path, "sarc");
        } else
            return al::loadArchiveWithExt(path, "szs");
    }

    void installFSHacks() {
        openFileHook.installAtSym<"_ZN2nn2fs8OpenFileEPNS0_10FileHandleEPKci">();
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x0080f270, loadArchiveHook);
    }

    static bool endsWith(const char* str, const char* suffix) {
        size_t strLen = strlen(str);
        size_t suffixLen = strlen(suffix);

        if (suffixLen > strLen) {
            return 0;
        }

        const char* strEnd = str + strLen - suffixLen;

        return strcmp(strEnd, suffix) == 0;
    }

    static mem patchFileData(mem patch, mem source) {
        mem patchedFile;
        bpserror error = bps_apply(patch, source, &patchedFile, nullptr, false);
        HK_ABORT_UNLESS(error == bpserror::bps_ok, "BPS patching failed with bpserror: %d", error);
        return patchedFile;
    }

    static void patchFile(const char* bpsPath, const char* originalPath, const char* outPath) {
        HK_ABORT_UNLESS(FsHelper::isFileExist(originalPath), "Patch %s exists, but source file %s does not!", bpsPath, originalPath);

        nn::fs::FileHandle patchFile;
        HK_ASSERT(nn::fs::OpenFile(&patchFile, bpsPath, nn::fs::OpenMode_Read).IsSuccess());
        s64 patchSize = 0;
        HK_ASSERT(nn::fs::GetFileSize(&patchSize, patchFile).IsSuccess());
        u8* patchData = (u8*)sPatchHeap->alloc(patchSize);
        nn::fs::ReadFile(patchFile, 0, patchData, patchSize);
        nn::fs::CloseFile(patchFile);

        nn::fs::FileHandle sourceFile;
        HK_ASSERT(nn::fs::OpenFile(&sourceFile, originalPath, nn::fs::OpenMode_Read).IsSuccess());
        s64 sourceSize = 0;
        HK_ASSERT(nn::fs::GetFileSize(&sourceSize, sourceFile).IsSuccess());
        u8* sourceData = (u8*)sPatchHeap->alloc(sourceSize);
        nn::fs::ReadFile(sourceFile, 0, sourceData, sourceSize);
        nn::fs::CloseFile(sourceFile);

        bool isYaz0 = endsWith(originalPath, ".szs");

        if (isYaz0) {
            struct header {
                u8 magic[4];
                u32 decompressedSize;
            }* header(reinterpret_cast<struct header*>(sourceData));

            u32 decompressedSize = sead::Endian::swapU32(header->decompressedSize);
            u8* decompressedData = (u8*)sPatchHeap->alloc(decompressedSize);
            HK_ABORT_UNLESS(decodeSZSNxAsm64_(decompressedData, sourceData) >= 0, "SZS decompression failed!", 0);

            sPatchHeap->free(sourceData);
            sourceData = decompressedData;
            sourceSize = decompressedSize;
        }

        mem patchedFile = patchFileData({ patchData, size_t(patchSize) }, { sourceData, size_t(sourceSize) });
        sPatchHeap->free(patchData);
        sPatchHeap->free(sourceData);

        FsHelper::writeFileToPath(patchedFile.ptr, patchedFile.len, outPath);

        sPatchHeap->free(patchedFile.ptr);
    }

    constexpr int maxPathLength = nn::fs::PathLengthMax * 2 + 1;

    static sead::FixedSafeString<maxPathLength> getParentPath(const char* filePath) {
        sead::FixedSafeString<maxPathLength> out(filePath);
        s32 idx = out.rfindIndex("/");
        if (idx == -1)
            return { filePath };
        out.trim(idx);
        return out;
    }

    static bool directoryExists(const char* dirPath) {
        nn::fs::DirectoryEntryType type = (nn::fs::DirectoryEntryType)2;
        return nn::fs::GetEntryType(&type, dirPath).IsSuccess() && type == nn::fs::DirectoryEntryType_Directory;
    }

    static void createDirectoryRecursively(const char* dirPath) {
        sead::FixedSafeString<maxPathLength> path(dirPath);

        while (!directoryExists(path.cstr())) {
            auto parent = getParentPath(path.cstr());
            if (directoryExists(parent.cstr())) {
                nn::fs::CreateDirectory(path.cstr());
                createDirectoryRecursively(dirPath);
            }
            path = parent;
        }
    }

    static void patchDirRecursive(const char* rootWalkPath, const char* walkPath) {
        pe::log("Applying RomFS patches to %s", walkPath);

        nn::fs::DirectoryHandle dirHandle;
        sead::FormatFixedSafeString<maxPathLength>* actualWalkPath = new sead::FormatFixedSafeString<maxPathLength>("%s%s", walkPath, endsWith(walkPath, ":") ? "/" : "");
        nn::fs::OpenDirectory(&dirHandle, actualWalkPath->cstr(), nn::fs::OpenDirectoryMode_All);
        s64 entryCount = -1;
        nn::fs::GetDirectoryEntryCount(&entryCount, dirHandle);

        nn::fs::DirectoryEntry* entries = (nn::fs::DirectoryEntry*)sPatchHeap->alloc(sizeof(nn::fs::DirectoryEntry) * entryCount);
        nn::fs::ReadDirectory(&entryCount, entries, dirHandle, entryCount);
        for (s64 i = 0; i < entryCount; i++) {
            nn::fs::DirectoryEntry& entry = entries[i];
            sead::FormatFixedSafeString<maxPathLength>* entryPath = new sead::FormatFixedSafeString<maxPathLength>("%s/%s", walkPath, entry.name);

            if (entry.type == nn::fs::DirectoryEntryType_File) {
                if (entryPath->endsWith(".bps")) {
                    sead::FixedSafeString<maxPathLength>* rawPath = new sead::FixedSafeString<maxPathLength>(*entryPath);
                    rawPath->removeSuffix(".bps");

                    sead::FixedSafeString<maxPathLength>* originalPath = new sead::FixedSafeString<maxPathLength>(*rawPath);
                    rawPath->replaceString(rootWalkPath, "");

                    sead::FormatFixedSafeString<maxPathLength>* outPath = new sead::FormatFixedSafeString<maxPathLength>("sd:/BowsersFuryOnline/cache/%s", rawPath->cstr() + 1);
                    if (rawPath->endsWith(".szs")) {

                        outPath->removeSuffix(".szs");
                        outPath->append(".sarc");
                    }

                    pe::log("Patching %s with %s to %s", originalPath->cstr(), entryPath->cstr(), outPath->cstr());
                    createDirectoryRecursively(getParentPath(outPath->cstr()).cstr());
                    patchFile(entryPath->cstr(), originalPath->cstr(), outPath->cstr());

                    delete originalPath;
                    delete outPath;
                    delete rawPath;
                }
            } else
                patchDirRecursive(rootWalkPath, entryPath->cstr());

            delete entryPath;
        }
        delete actualWalkPath;

        sPatchHeap->free(entries);

        nn::fs::CloseDirectory(dirHandle);
    }

    void applyRomFSPatches() {
        sPatchHeap = sead::ExpHeap::create(0, "PatchHeap", al::getSequenceHeap(), 8, sead::ExpHeap::cHeapDirection_Forward, false);

        sHookDisabled = true;
        sead::ScopedCurrentHeapSetter setter(sPatchHeap);
        nn::fs::DeleteDirectoryRecursively("sd:/BowsersFuryOnline/cache");
        createDirectoryRecursively("sd:/BowsersFuryOnline/cache");
        patchDirRecursive("content:", "content:");
        sHookDisabled = false;

        sPatchHeap->destroy();
        sPatchHeap = nullptr;
    }

} // namespace pe
