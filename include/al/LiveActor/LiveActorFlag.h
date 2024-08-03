#pragma once

namespace al {

class LiveActor;
struct LiveActorFlag {
    bool isDead = true;
    bool isClipped = false;
    bool flag3;
    bool isDraw;
    bool isClippedByLod;
    bool isOffCalcAnim;
    bool isHideModel = false;
    bool isColliderDisabled;
    bool flag9;
    bool flag10;
    bool isMaterialCode;
    bool isAreaTarget;
    bool isUpdateMovementEffectAudioCollisionSensor = true; // true?
};

bool isDead(const LiveActor* actor);
bool isClipped(const LiveActor* actor);
bool isHideModel(const LiveActor* actor);

void hideModelIfShow(LiveActor* actor);
void onCollide(LiveActor* actor);
void offCollide(LiveActor* actor);

} // namespace al
