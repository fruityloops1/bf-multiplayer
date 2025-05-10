#include "pe/Multiplayer/Puppets/PuppetSensorActor.h"
#include "Game/Util/SensorUtil.h"
#include "al/HitSensor/HitSensor.h"
#include "al/HitSensor/SensorMsg.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "pe/Util/Log.h"

namespace pe {

void PuppetSensorActor::init(const al::ActorInitInfo& info)
{
    al::initActorWithArchiveName(this, info, "PlayerPuppet");

    makeActorAppeared();
}

void PuppetSensorActor::attackSensor(al::HitSensor* me, al::HitSensor* other)
{
    if (al::isSensorEnemyBody(me) && !al::sendMsgPush(other, me))
        rc::sendMsgPushConnected(other, me);
}

} // namespace pe
