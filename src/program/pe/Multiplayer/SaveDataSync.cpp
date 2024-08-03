#include "pe/Multiplayer/SaveDataSync.h"
#include "al/Base/String.h"
#include "al/Functor/FunctorV0M.h"
#include "al/System/SaveData.h"
#include "al/Thread/AsyncFunctorThread.h"
#include "heap/seadDisposer.h"
#include "heap/seadHeapMgr.h"
#include "helpers/fsHelper.h"
#include "hook/trampoline.hpp"
#include "nn/fs.h"
#include "patch/code_patcher.hpp"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Enet/NetClient.h"
#include "pe/Enet/Packets/DataPackets.h"
#include "pe/Util/Log.h"
#include "util/modules.hpp"
#include "util/sys/rw_pages.hpp"
#include <limits>
#include <sead/basis/seadTypes.h>

namespace pe {

SaveDataMode& getSaveDataMode()
{
    static SaveDataMode mode = SaveDataMode::Local;
    return mode;
}

static constexpr char sCachedSaveFilePath[] = "sd:/BowsersFuryOnline/cache/GameDataRemote.bin";
static u8 sCachedSaveFileWorkBuffer[std::numeric_limits<u16>::max() + 0x1000] { 0 };
static bool sCachedSaveFileAck[8] { false };
static size_t sCachedSaveFileSize = 0;

const u8* getSaveDataWorkBuffer()
{
    return sCachedSaveFileWorkBuffer;
}

size_t getSaveDataWorkBufferSize()
{
    return sCachedSaveFileSize;
}

const bool* getSaveDataAckBuffer()
{
    return sCachedSaveFileAck;
}

void resetSaveDataBuffer()
{
    memset(sCachedSaveFileWorkBuffer, 0, sizeof(sCachedSaveFileWorkBuffer));
    memset(sCachedSaveFileAck, 0, sizeof(sCachedSaveFileAck));
    sCachedSaveFileSize = 0;
}

void handle_SaveDataChunk(enet::SaveDataChunk* packet)
{
    if (packet->chunkIndex < 0 || packet->chunkIndex >= 8)
        return;
    if (packet->size > enet::sSaveDataChunkSize)
        return;

    if (sCachedSaveFileAck[packet->chunkIndex] == false)
        sCachedSaveFileSize += packet->size;
    memcpy(sCachedSaveFileWorkBuffer + enet::sSaveDataChunkSize * packet->chunkIndex, packet->data, packet->size);
    sCachedSaveFileAck[packet->chunkIndex] = true;
}

class SaveDataUploadThread : public al::AsyncFunctorThread {
    nn::os::Event mEvent;

    SEAD_SINGLETON_DISPOSER(SaveDataUploadThread)
public:
    SaveDataUploadThread()
        : AsyncFunctorThread("SaveDataUploadThread", al::FunctorV0M(this, &SaveDataUploadThread::threadFunc), 1, 0x4000)
    {
        nn::os::InitializeEvent(&mEvent, false, nn::os::EventClearMode_AutoClear);
        start();
    }

    void threadFunc()
    {
        while (true) {
            nn::os::WaitEvent(&mEvent);

            for (int i = 0; i < 8; i++) {
                enet::SaveDataChunk packet;
                packet.chunkIndex = i;
                packet.size = i == 7 ? (sCachedSaveFileSize - enet::sSaveDataChunkSize * 7) : enet::sSaveDataChunkSize;
                memcpy(packet.data, sCachedSaveFileWorkBuffer + enet::sSaveDataChunkSize * i, packet.size);
                enet::getNetClient()->sendPacket(&packet, true);
                nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(70));
            }
        }
    }

    void requestUpload()
    {
        nn::os::SignalEvent(&mEvent);
    }
};

SEAD_SINGLETON_DISPOSER_IMPL(SaveDataUploadThread)

static void requestUploadSaveData()
{
    sead::ScopedCurrentHeapSetter setter(pe::gui::getPeepaHeap());
    if (SaveDataUploadThread::instance() == nullptr) {
        SaveDataUploadThread::createInstance(nullptr);
    }

    SaveDataUploadThread::instance()->requestUpload();
}

HOOK_DEFINE_TRAMPOLINE(SaveDataSequenceReadThread)
{
    static int Callback(al::SaveDataSequenceRead * thisPtr, const char* path);
};

int SaveDataSequenceReadThread::Callback(al::SaveDataSequenceRead* thisPtr, const char* path)
{
    if (getSaveDataMode() == SaveDataMode::Local)
        return Orig(thisPtr, path);
    else {
        if (thisPtr->mSaveBufferSize == 0)
            return 0;

        nn::fs::FileHandle handle;
        nn::fs::OpenFile(&handle, sCachedSaveFilePath, nn::fs::OpenMode_Read);
        s64 readSize;
        nn::fs::GetFileSize(&readSize, handle);
        nn::fs::ReadFile(handle, 0, thisPtr->mSaveBuffer, thisPtr->mSaveBufferSize);
        nn::fs::CloseFile(handle);
        return 0;
    }
}

HOOK_DEFINE_TRAMPOLINE(SaveDataSequenceWriteThread) { static int Callback(al::SaveDataSequenceWrite * thisPtr, const char* path); };

int SaveDataSequenceWriteThread::Callback(al::SaveDataSequenceWrite* thisPtr, const char* path)
{
    if (getSaveDataMode() == SaveDataMode::Local)
        return Orig(thisPtr, path);
    else {
        if (thisPtr->mSaveBufferSize == 0)
            return 0;

        nn::fs::FileHandle handle;
        if (FsHelper::isFileExist(sCachedSaveFilePath))
            nn::fs::CreateFile(sCachedSaveFilePath, thisPtr->mSaveBufferSize);
        nn::fs::OpenFile(&handle, sCachedSaveFilePath, nn::fs::OpenMode_Write);
        nn::fs::SetFileSize(handle, thisPtr->mSaveBufferSize);
        nn::fs::WriteFile(handle, 0, thisPtr->mSaveBuffer, thisPtr->mSaveBufferSize, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
        nn::fs::CloseFile(handle);

        memcpy(sCachedSaveFileWorkBuffer, thisPtr->mSaveBuffer, thisPtr->mSaveBufferSize);
        sCachedSaveFileSize = thisPtr->mSaveBufferSize;
        requestUploadSaveData();
        return 0;
    }
}

static const char* getSaveFile()
{
    return "GameData.bin";
    // return getSaveDataMode() == SaveDataMode::Local ? "GameData.bin" : "BowsersFuryOnline/cache/GameDataRemote.bin";
}

static void requestWriteSaveDataHook(const char* file, u32 a, u32 b, bool c)
{
    al::requestWriteSaveData(getSaveFile(), a, b, c);
}

static void requestReadSaveDataHook(const char* file, u32 a, u32 b)
{
    al::requestReadSaveData(getSaveFile(), a, b);
}

static void requestInitSaveDataHook(const char* file, u32 a, u32 b)
{
    al::requestInitSaveDir(getSaveFile(), a, b);
}

void installSaveDataSyncHooks()
{
    using Patcher = exl::patch::CodePatcher;
    Patcher(0x0041b2a4).BranchLinkInst((void*)requestWriteSaveDataHook);
    Patcher(0x0041b198).BranchLinkInst((void*)requestReadSaveDataHook);
    Patcher(0x0041b47c).BranchLinkInst((void*)requestInitSaveDataHook);
    SaveDataSequenceReadThread::InstallAtOffset(0x008a2fa0);
    SaveDataSequenceWriteThread::InstallAtOffset(0x008a3210);
}

} // namespace pe
