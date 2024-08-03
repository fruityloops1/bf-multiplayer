#pragma once

#include <sead/math/seadVectorFwd.h>

namespace al {

class ActorInitInfo;
class HitSensor;
class LiveActor;

void addHitSensor(LiveActor* actor, const ActorInitInfo& info, const char* name, u32 sensorType, float radius, u16 maxCount, const sead::Vector3f& offset);
HitSensor* getHitSensor(const LiveActor* actor, const char* sensorName);
LiveActor* getSensorHost(const HitSensor* sensor);

} // namespace al
