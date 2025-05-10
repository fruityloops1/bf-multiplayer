#include "al/HitSensor/SensorMsg.h"
#include "al/HitSensor/HitSensor.h"

namespace al {

bool isSensorMultiPlayer(const HitSensor* sensor)
{
    return sensor->getType() == HitSensorType::MultiPlayer;
}

} // namespace al
