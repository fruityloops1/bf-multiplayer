#pragma once

#include "pe/Enet/Packets/DataPackets.h"

namespace pe {

enum class SaveDataMode {
    Local,
    Server
};

SaveDataMode& getSaveDataMode();
const u8* getSaveDataWorkBuffer();
size_t getSaveDataWorkBufferSize();
const bool* getSaveDataAckBuffer();
void resetSaveDataBuffer();

void handle_SaveDataChunk(enet::SaveDataChunk* packet);

void installSaveDataSyncHooks();

} // namespace pe
