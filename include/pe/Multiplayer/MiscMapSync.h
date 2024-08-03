#pragma once

#include "pe/Enet/Packets/DataPackets.h"

namespace pe {

void installMiscMapSyncHooks();
void handleToC_MiscMapSync(enet::ToC_MiscMapSync* packet);
void handleToC_JumpFlip(enet::ToC_JumpFlip* packet);

} // namespace pe
