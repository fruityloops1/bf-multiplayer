#pragma once

namespace al {

class ActorInitInfo;
class LiveActor;

void validateClipping(LiveActor* actor);
void invalidateClipping(LiveActor* actor);

void initActorClipping(LiveActor* actor, const ActorInitInfo& info);

} // namespace al
