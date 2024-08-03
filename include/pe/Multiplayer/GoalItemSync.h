#pragma once

#include "pe/Enet/Packets/DataPackets.h"

namespace pe {

void initGoalItemSyncHooks();
void handleToC_GoalItemInfo(enet::ToC_GoalItemInfo* packet);
void handleToC_PhaseUnlock(enet::ToC_PhaseUnlock* packet);
void handleToC_ScenarioUpdate(enet::ToC_ScenarioUpdate* packet);

} // namespace pe
