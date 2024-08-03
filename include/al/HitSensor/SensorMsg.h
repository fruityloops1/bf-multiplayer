#pragma once

#include "al/HitSensor/HitSensor.h"

namespace al {

class SensorMsg {
};

bool isMsgPlayerFloorTouch(const SensorMsg* msg);
bool isMsgTouchAssist(const SensorMsg* msg);
bool isMsgPlayerTrampleForCrossoverSensor(const SensorMsg* msg, HitSensor* other, HitSensor* me);

bool sendMsgEnemyAttack(HitSensor* to, HitSensor* from);
bool sendMsgPush(HitSensor* to, HitSensor* from);
bool sendMsgPlayerHipDropKnockDown(HitSensor* to, HitSensor* from);

bool isSensorPlayer(const HitSensor* sensor);
bool isSensorMultiPlayer(const HitSensor* sensor);
bool isSensorEnemyBody(const HitSensor* sensor);

} // namespace al
