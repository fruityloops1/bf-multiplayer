#pragma once

#include "pe/Enet/Packets/DataPackets.h"

namespace pe {

void handleToC_DisasterModeControllerInternalUpdate(enet::ToC_DisasterModeControllerInternalUpdate* packet);

void installDisasterSyncHooks();

} // namespace pe
