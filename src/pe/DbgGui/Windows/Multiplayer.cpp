#include "pe/DbgGui/Windows/Multiplayer.h"
#include "Game/MapObj/DisasterModeController.h"
#include "Game/Sequence/ProductStateSingleMode.h"
#include "Game/System/GameDataHolderAccessor.h"
#include "al/Nerve/NerveFunction.h"
#include "imgui.h"
#include "pe/Enet/Channels.h"
#include "pe/Enet/PacketHandler.h"
#include "pe/Enet/Types.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Multiplayer/MultiplayerStatusText.h"
#include "pe/Util/Nerve.h"
#include <sead/math/seadMathCalcCommon.h>

namespace pe {
namespace gui {

    void Multiplayer::update() { mMgr = MultiplayerManager::instance(); }
    void Multiplayer::draw()
    {
        if (getDbgGuiSharedData().showMultiplayer) {
            if (ImGui::Begin("Multiplayer", &getDbgGuiSharedData().showMultiplayer) && mMgr) {

                if (ImGui::CollapsingHeader("Players"))
                    for (PlayerData& player : mMgr->mPlayers) {
                        if (ImGui::TreeNode(player.getListData().name)) {
                            ImGui::Text("Has puppet: %s", player.mPlayerPuppet != nullptr ? "true" : "false");
                            ImGui::Text("UUID:");
                            ImGui::SameLine();
                            PE_PRINT_UUID(player.getListData().user, ImGui::Text);
                            ImGui::Text("Is host client: %s", player.mIsHostClient ? "true" : "false");
                            if (!player.isLocal()) {
                                const sead::Vector3f& trans = player.getPoseData().trans;
                                const sead::Quatf& quat = player.getPoseData().quat;
                                ImGui::Text("Trans: %.2f %.2f %.2f", trans.x, trans.y, trans.z);
                                ImGui::Text("Quat: %.2f %.2f %.2f %.2f", quat.w, quat.x, quat.y, quat.z);
                                const sead::Vector3f& lastTrans = player.mLastTrans;
                                const sead::Quatf& lastQuat = player.mLastQuat;
                                ImGui::Text("Last Trans: %.2f %.2f %.2f", lastTrans.x, lastTrans.y, lastTrans.z);
                                ImGui::Text("Last Quat: %.2f %.2f %.2f %.2f", lastQuat.w, lastQuat.x, lastQuat.y, lastQuat.z);
                                if (ImGui::Button("End Sub Anim")) {
                                    player.mPlayerPuppet->mPlayerAnimator->endSubAnim();
                                }
                                if (ImGui::Button("Reset Weight")) {
                                    player.mPlayerPuppet->mPlayerAnimator->resetWeight();
                                }
                                ImGui::Text("Pose interval: %dms", int(player.mPoseUpdateIntervalTicks / (nn::os::GetSystemTickFrequency() / 1000.f)));
                            }
                            ImGui::TreePop();
                        }
                    }
                if (ImGui::CollapsingHeader("Packets")) {
                    if (ImGui::TreeNode("Sent")) {
                        for (int i = 0; i < enet::sChannels.channelCount; i++)
                            ImGui::Text("%s: %d", enet::sChannels.packetNames[i], mMgr->getClient()->getPacketHandler()->getPacketsSent((enet::ChannelType)i));
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Received")) {
                        for (int i = 0; i < enet::sChannels.channelCount; i++)
                            ImGui::Text("%s: %d", enet::sChannels.packetNames[i], mMgr->getClient()->getPacketHandler()->getPacketsReceived((enet::ChannelType)i));
                        ImGui::TreePop();
                    }
                }

                MultiplayerStatusText* text = mMgr->mStatusText;
                if (text && ImGui::Button("Test Status Text")) {
                    text->log(u"HELLO !!!");
                }

                ProductSequence* sequence = mMgr->getSequence();
                al::Scene* scene = sequence->tryGetCurrentScene();
                if (scene && ImGui::CollapsingHeader("SingleModeData")) {
                    GameDataHolderAccessor accessor(scene);

                    ImGui::DragInt("Num Goal Items", &accessor.mHolder->mData->mNumGoalItems);
                    ImGui::DragInt("Unlocked Phase", &accessor.mHolder->mData->mUnlockedPhase);
                    ImGui::Checkbox("Is End Phase", &accessor.mHolder->mData->mIsPhaseEnd);
                    ImGui::DragInt("Stage Idx", &sequence->mStateSingleMode->mStageIndex);
                    ImGui::Checkbox("IsNewToPhase1", &accessor.mHolder->mData->mIsNewToPhase1);
                    ImGui::Checkbox("IsNewToPhase2", &accessor.mHolder->mData->mIsNewToPhase2);
                    ImGui::Checkbox("IsNewToPhase3", &accessor.mHolder->mData->mIsNewToPhase3);
                    ImGui::Checkbox("IsNewToPhase4", &accessor.mHolder->mData->mIsNewToPhase4);
                    ImGui::Checkbox("IsNewToPhase2Boss", &accessor.mHolder->mData->mIsNewToPhase2Boss);
                    ImGui::Checkbox("IsNewToPhase3Boss", &accessor.mHolder->mData->mIsNewToPhase3Boss);
                    ImGui::Checkbox("IsNewToPhase1BowserIntro", &accessor.mHolder->mData->mIsNewToPhase1BowserIntro);
                    ImGui::Checkbox("IsNewToPhase2BowserIntro", &accessor.mHolder->mData->mIsNewToPhase2BowserIntro);
                    ImGui::Checkbox("IsNewToPhase3BowserIntro", &accessor.mHolder->mData->mIsNewToPhase3BowserIntro);
                    ImGui::Checkbox("IsNewToPhase1BowserExit", &accessor.mHolder->mData->mIsNewToPhase1BowserExit);
                    ImGui::Checkbox("IsNewToPhase2BowserExit", &accessor.mHolder->mData->mIsNewToPhase2BowserExit);
                    ImGui::Checkbox("IsNewToPhase3BowserExit", &accessor.mHolder->mData->mIsNewToPhase3BowserExit);
                    ImGui::DragInt("Phase1DarkBowserHitPoint", &accessor.mHolder->mData->mPhase1DarkBowserHitPoint);
                    ImGui::DragInt("Phase2DarkBowserHitPoint", &accessor.mHolder->mData->mPhase2DarkBowserHitPoint);
                    ImGui::DragInt("Phase3DarkBowserHitPoint", &accessor.mHolder->mData->mPhase3DarkBowserHitPoint);
                    ImGui::DragInt("Phase4DarkBowserHitPoint", &accessor.mHolder->mData->mPhase4DarkBowserHitPoint);
                    ImGui::DragInt("Phase4DarkBowserHitPointFinal", &accessor.mHolder->mData->mPhase4DarkBowserHitPointFinal);
                    ImGui::DragInt("Phase3DarkBowserHitPointPreBattle", &accessor.mHolder->mData->mPhase3DarkBowserHitPointPreBattle);
                    ImGui::DragInt("Phase4DarkBowserHitPointPreBattle", &accessor.mHolder->mData->mPhase4DarkBowserHitPointPreBattle);
                }

                if (ImGui::CollapsingHeader("DisasterModeController")) {
                    if (scene) {
                        DisasterModeController* controller = DisasterModeController::tryGetController(scene);
                        ImGui::DragInt("FramesOfProsperityTransition", &controller->mFramesOfProsperityTransition);
                        ImGui::DragInt("FramesOfProsperity", &controller->mFramesOfProsperity);
                        ImGui::DragInt("FramesOfAnticipationTransition", &controller->mFramesOfAnticipationTransition);
                        ImGui::DragInt("FramesOfAnticipation", &controller->mFramesOfAnticipation);
                        ImGui::DragInt("FramesOfDisasterTransition", &controller->mFramesOfDisasterTransition);
                        ImGui::DragInt("FramesOfDisaster", &controller->mFramesOfDisaster);
                        ImGui::DragInt("FramesUntilDisaster", &controller->mFramesUntilDisaster);
                        ImGui::DragInt("ProgressFrames", &controller->mProgressFrames);
                        ImGui::Checkbox("Disable Clock", &controller->mDisableClock);

                        const std::pair<uintptr_t, const char*> shit[] {
                            { 0x01318b68, "ProsperityTransitionWipeOut" },
                            { 0x01318b58, "ProsperityTransitionWipeIn" },
                            { 0x01318b40, "Prosperity" },
                            { 0x01318b70, "PostBattlePeaceDelay" },
                            { 0x01318b38, "FirstAppearCutscene" },
                            { 0x01318b30, "Disaster" },
                            { 0x01318ac0, "DisasterTransitionWipeOut" },
                            { 0x01318930, "DisasterTransitionWipeInDemo" },
                            { 0x01318ae8, "DisasterTransitionWipeOutDemo" },
                            { 0x01318b50, "DisasterTransitionInstant" },
                            { 0x01318b80, "AnticipationTransition" },
                            { 0x01318b48, "AnticipationFast" },
                            { 0x01318b60, "Anticipation" },
                        };

                        if (ImGui::Button("End Disaster")) {
                            controller->endInstantly(true, true);
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Start Disaster")) {
                            controller->beginImmediate(true);
                        }

                        bool foundNrv = false;

                        for (auto entry : shit) {
                            const al::Nerve* nrv = pe::util::getNerveAt(entry.first);
                            if (al::isNerve(controller, nrv)) {
                                ImGui::Text("Nrv: %s", entry.second);
                                foundNrv = true;
                                break;
                            }
                        }

                        if (!foundNrv)
                            ImGui::Text("Nrv: %p", controller->getNerveKeeper()->getCurrentNerve());
                    }
                }
            }

            ImGui::End();
        }
    }

} // namespace gui
} // namespace pe
