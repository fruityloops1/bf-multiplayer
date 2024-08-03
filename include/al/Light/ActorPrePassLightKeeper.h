#pragma once

namespace al {

class LiveActor;
class ActorPrePassLightKeeper {
};

void killPrePassLightAll(const al::LiveActor* actor, int);
void appearPrePassLight(const al::LiveActor* actor, const char*, int);

} // namespace al