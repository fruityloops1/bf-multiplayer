#include "pe/Multiplayer/Puppets/RaidonPuppet.h"
#include "Game/System/GameDataHolderAccessor.h"
#include "Game/System/SingleModeDataFunction.h"
#include "al/Effect/EffectKeeper.h"
#include "al/LiveActor/ActorClippingFunction.h"
#include "al/LiveActor/LiveActorFlag.h"
#include "al/Se/SeFunction.h"
#include "pe/Multiplayer/PlayerData.h"

namespace pe::puppets {

RaidonPuppet::RaidonPuppet()
    : LiveActor("Plessie Puppet")
{
}

RaidonPuppet::~RaidonPuppet()
{
    if (mPlayerData)
        mPlayerData->setRaidonPuppet(nullptr);
}

void RaidonPuppet::init(const al::ActorInitInfo& info)
{
    // NO clue why this shit crashes
    // GameDataHolderAccessor accessor(info.getSceneInfo()->mSceneObjHolder);
    // int unlockedPhase = SingleModeDataFunction::getUnlockedPhase(accessor);
    int unlockedPhase = 0;
    al::initActorWithArchiveName(this, info, unlockedPhase < 8 ? "RaidonSurf" : "RaidonSurfFur");
    al::invalidateClipping(this);
}

void RaidonPuppet::appear()
{
    al::tryEmitEffect(this, "Poof", nullptr);
    makeActorAppeared();
}

void RaidonPuppet::kill()
{
    al::startSe(this, "PgVanish", nullptr);
    al::tryEmitEffect(this, "Poof", nullptr);
    makeActorDead();
}

} // namespace pe::puppets
