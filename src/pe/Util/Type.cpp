#include "pe/Util/Type.h"
#include "Game/Boss/DarkBowser.h"
#include "Game/Enemy/ShadowMario.h"
#include "Game/Enemy/ShadowRacer.h"
#include "Game/MapObj/GoalItem.h"
#include "Game/MapObj/JumpFlipPanel.h"
#include "Game/MapObj/Shards.h"
#include "Game/Npc/NekoNormal.h"
#include "Game/Npc/NekoParent.h"
#include "Game/Player/CourseSelectPlayerActor.h"
#include "Game/Player/PlayerActor.h"
#include "Game/Scene/PhaseBossScene.h"

namespace pe {
namespace util {

    template <>
    bool checkTypeByVtable<PlayerActor>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x0137a038;
    }

    template <>
    bool checkTypeByVtable<CourseSelectPlayerActor>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x0128a7d0;
    }

    template <>
    bool checkTypeByVtable<GoalItem>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x01327de8;
    }

    template <>
    bool checkTypeByVtable<ShadowMario>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x012cdbd0;
    }

    template <>
    bool checkTypeByVtable<ShadowRacer>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x012ceae8;
    }

    template <>
    bool checkTypeByVtable<Shards>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x01350388;
    }

    template <>
    bool checkTypeByVtable<JumpFlipPanel>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x01333c40;
    }

    template <>
    bool checkTypeByVtable<DarkBowser>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x0126db30;
    }

    template <>
    bool checkTypeByVtable<NekoParent>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x01370380;
    }

    template <>
    bool checkTypeByVtable<NekoNormal>(const al::LiveActor* actor)
    {
        return getVftOffsetMain(actor) == 0x0136f3b0;
    }

    template <>
    bool checkTypeByVtable<PhaseBossScene>(const SingleModeScene* scene)
    {
        return getVftOffsetMain(scene) == 0x01386c50;
    }

    PlayerActor* checkPlayer(al::LiveActor* instance)
    {
        if (pe::util::checkTypeByVtable<PlayerActor>(instance) or pe::util::checkTypeByVtable<CourseSelectPlayerActor>(instance))
            return static_cast<PlayerActor*>(instance);

        return nullptr;
    }

} // namespace util
} // namespace pe
