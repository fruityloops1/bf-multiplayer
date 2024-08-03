#pragma once

#include "al/LiveActor/LiveActor.h"

namespace rc {

bool isPlayerHideModel(const al::LiveActor* actor);
bool isPlayerGiga(const al::LiveActor* actor);
bool isAnyPlayerJumpTrigOn(const al::LiveActor* actor);

} // namespace rc
