#pragma once

#include <sead/basis/seadTypes.h>

namespace al {

class SaveDataSequenceBase {
public:
    virtual void threadFunc(const char* saveFilePath);

    u8* mSaveBuffer;
    u32 mSaveBufferSize;
};

class SaveDataSequenceWrite : public SaveDataSequenceBase {
public:
    void threadFunc(const char* saveFilePath) override;
};

class SaveDataSequenceRead : public SaveDataSequenceBase {
public:
    void threadFunc(const char* saveFilePath) override;
};

void requestInitSaveDir(const char* file, u32, u32);
void requestWriteSaveData(const char* file, u32, u32, bool);
void requestReadSaveData(const char* file, u32, u32);
bool updateSaveDataSequence();

} // namespace al
