#include "pe/Factory/ProjectActorFactory.h"
#include "patch/code_patcher.hpp"
#include "pe/Enemy/Foomin.h"
#include "pe/MapObj/CoinVideoPlayer.h"
#include "pe/MapObj/NeedleSwitchParts.h"
#include "pe/MapObj/PatanPanel.h"
#include "pe/MapObj/PatanPanelStarter.h"
#include "pe/Multiplayer/MapObj/ClockMapPartsNet.h"
#include "pe/Multiplayer/MapObj/KeyMoveMapPartsNet.h"
#include "pe/Multiplayer/MapObj/RotateMapPartsNet.h"
#include "pe/Multiplayer/Puppets/PuppetSensorActor.h"

constexpr static al::ActorFactoryTableEntry customActorEntries[] = {
    { "Foomin", pe::createActorFunction<pe::Foomin> },
    { "PatanPanel", pe::createActorFunction<pe::PatanPanel> },
    { "PatanPanelStarter", pe::createActorFunction<pe::PatanPanelStarter> },
    { "NeedleSwitchParts", pe::createActorFunction<pe::NeedleSwitchParts> },
    { "PushTest", pe::createActorFunction<pe::PuppetSensorActor> },

    // Multiplayer syncing actors
    { "ClockMapParts", pe::createActorFunction<pe::ClockMapPartsNet> },
    { "RotateMapParts", pe::createActorFunction<pe::RotateMapPartsNet> },
    { "KeyMoveMapParts", pe::createActorFunction<pe::KeyMoveMapPartsNet> },
};

pe::ProjectActorFactory::ProjectActorFactory()
{
    static al::ActorFactoryTableEntry
        outEntries[sizeof(customActorEntries) / sizeof(al::ActorFactoryTableEntry) + sizeof(::ProjectActorFactory::sActorEntries) / sizeof(al::ActorFactoryTableEntry)];
    static bool isInitialized = false;

    if (!isInitialized) {
        int i = 0;

        for (const al::ActorFactoryTableEntry& entry : customActorEntries) {
            outEntries[i] = entry;
            i++;
        }

        for (const al::ActorFactoryTableEntry& entry : ::ProjectActorFactory::sActorEntries) {
            outEntries[i] = entry;
            i++;
        }
        isInitialized = true;
    }

    initFactory(outEntries);
}

static void projectActorFactoryHook(ProjectActorFactory* factory) { new (factory) pe::ProjectActorFactory(); }

void pe::initProjectActorFactoryHook()
{
    using Patcher = exl::patch::CodePatcher;

    Patcher(0x003d86b0).BranchInst((void*)projectActorFactoryHook);
}