#include "pe/Multiplayer/DisasterSync.h"
#include "Game/Boss/DarkBowser.h"
#include "Game/MapObj/DisasterModeController.h"
#include "Game/MapObj/SuperBowserShell.h"
#include "Game/Scene/SingleModeScene.h"
#include "Game/Sequence/ProductStateSingleMode.h"
#include "Game/Util/SceneObjUtil.h"
#include "al/Layout/LayoutInitInfo.h"
#include "al/Npc/WipeSimple.h"
#include "hk/ro/RoUtil.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Log.h"
#include "pe/Util/Nerve.h"
#include <cstring>

namespace pe {

void handleToC_DisasterModeControllerInternalUpdate(enet::ToC_DisasterModeControllerInternalUpdate* packet)
{
    pe::MultiplayerManager* mgr = pe::MultiplayerManager::instance();
    if (mgr == nullptr) {
        pe::warn("DisasterModeControllerInternalUpdate but no mgr");
    }

    // clang-format off
    ProductSequence* sequence = mgr->getSequence();
    if (sequence == nullptr or !util::isSequenceSingleModeGameOnlyNrv(sequence))
    {
        pe::warn("DisasterModeControllerInternalUpdate but wrong nerve");
        return;
    }
    // clang-format on

    if (sequence->mStateSingleMode->mStageIndex == 0 /* SingleModeOceanPhase0Stage */ || sequence->mStateSingleMode->mStageIndex == 9)
        return;

    SingleModeScene* scene = reinterpret_cast<SingleModeScene*>(sequence->tryGetCurrentScene());
    if (scene == nullptr) {
        pe::warn("DisasterModeControllerInternalUpdate but no scene");
        return;
    }

    DisasterModeController* controller = DisasterModeController::tryGetController(scene->mLiveActorKit->mAllActors->mActors[0]);

    if (controller == nullptr)
        return;

    // yuzu
    if (*reinterpret_cast<uintptr_t*>(controller) == 0)
        return;
    if (**reinterpret_cast<uintptr_t**>(controller) == 0)
        return;

    const auto& data = packet->getData();

    bool hasRainStateChanged = controller->mIsRaining != data.mIsRaining;

    controller->mFramesOfDisaster = data.mFramesOfDisaster;
    controller->mFramesOfDisasterTransition = data.mFramesOfDisasterTransition;
    controller->mFramesOfAnticipation = data.mFramesOfAnticipation;
    controller->mFramesOfAnticipationTransition = data.mFramesOfAnticipationTransition;
    controller->mFramesOfProsperity = data.mFramesOfProsperity;
    controller->mFramesOfProsperityTransition = data.mFramesOfProsperityTransition;
    controller->mProgressFrames = data.mProgressFrames;
    controller->mFramesUntilDisaster = data.mFramesUntilDisaster;
    controller->mDisableClock = data.mDisableClock;
    controller->mIsRaining = data.mIsRaining;
    controller->mPostBossPeaceTime = data.mPostBossPeaceTime;

    const al::Nerve* controllerNerve = util::getNerveAt(packet->getData().controllerNerve);
    if (packet->getData().isNewNerve or !al::isNerve(controller, controllerNerve)) {
        if (controllerNerve == util::getNerveAt(0x01318b40) /* Prosperity */) {
            controller->endInstantly(true, true);
        } else if (controllerNerve == util::getNerveAt(0x01318b30) /* Disaster */)
            controller->beginImmediate(true);
        else
            al::setNerve(controller, controllerNerve);
    }
    if (hasRainStateChanged)
        controller->updateRainEffects();
}

constexpr u32 cWipeRemove7[] { 0xD503201F };
constexpr u32 cWipeRemove8[] { 0x52800020 };
constexpr u32 cWipeRemove9[] { 0xD503201F };
constexpr u32 cWipeRemove10[] { 0x52800020 };

void installDisasterSyncHooks()
{
    hk::ro::getMainModule()->writeRo(0x0020f6b0, cWipeRemove7, sizeof(cWipeRemove7));
    hk::ro::getMainModule()->writeRo(0x0020f710, cWipeRemove8, sizeof(cWipeRemove8));
    hk::ro::getMainModule()->writeRo(0x0020f1c4, cWipeRemove9, sizeof(cWipeRemove9));
    hk::ro::getMainModule()->writeRo(0x0020f164, cWipeRemove10, sizeof(cWipeRemove10));
}

} // namespace pe
