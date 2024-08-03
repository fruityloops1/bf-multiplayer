#pragma once

namespace al {

class ActorInitInfo;
class LiveActor;

void registerAreaHostMtx(const LiveActor* actor, const ActorInitInfo& info);

} // namespace al
