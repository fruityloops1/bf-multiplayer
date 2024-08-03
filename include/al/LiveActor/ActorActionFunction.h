#pragma once

#include "al/Anim/SklAnimRetargettingInfo.h"

namespace al {

class LiveActor;

void startAction(LiveActor* actor, const char* actionName);
bool tryStartActionIfNotPlaying(LiveActor* actor, const char* actionName);
bool tryStartAction(LiveActor* actor, const char* actionName);
bool tryStartEffectAction(LiveActor* actor, const char* actionName);
const char* getActionName(const LiveActor* actor);
float getActionFrame(const LiveActor* actor);
float getActionFrameMax(const LiveActor* actor, const char* actionName);
bool isActionEnd(const LiveActor* actor);
bool isExistAction(const LiveActor* actor);
bool isExistAction(const LiveActor* actor, const char* actionName);
void startNerveAction(LiveActor* actor, const char* actionName);
void setSklAnimFrameRate(LiveActor* actor, float frameRate, int);
float getSklAnimFrameRate(const LiveActor* actor, int);
bool isSklAnimExist(const LiveActor* actor, const char* animName);
bool isSklAnimExist(const LiveActor* actor);
void addPartialSklAnimPartsListRecursive(LiveActor* actor, const char* jointName, int);
void clearSklAnimInterpole(LiveActor* actor);
void bindSklAnimRetargetting(const LiveActor* actor, const SklAnimRetargettingInfo* info); // inlined
void validateSklAnimRetargetting(const LiveActor* actor);
void invalidateSklAnimRetargetting(const LiveActor* actor);

void startHitReaction(const LiveActor* actor, const char* name);
void startHitReactionDisappear(const LiveActor* actor);

void setSklAnimBlendWeightSixfold(LiveActor* actor, float, float, float, float, float, float);

} // namespace al
