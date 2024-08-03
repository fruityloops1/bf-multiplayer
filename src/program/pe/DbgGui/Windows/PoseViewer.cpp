#include "pe/DbgGui/Windows/PoseViewer.h"
#include "Game/MapObj/DisasterModeController.h"
#include "Game/Player/PlayerActionNode.h"
#include "Game/Player/PlayerActor.h"
#include "Game/Util/SceneObjUtil.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/Nerve/NerveFunction.h"
#include "hook/trampoline.hpp"
#include "imgui.h"
#include "patch/code_patcher.hpp"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Util/Nerve.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/Type.h"

namespace pe {
namespace gui {

    PoseViewer::PoseViewer()
    {
    }

    void PoseViewer::update()
    {
    }

    void PoseViewer::draw()
    {
        if (getDbgGuiSharedData().showPoseViewer) {
            if (ImGui::Begin("PoseViewer", &getDbgGuiSharedData().showPoseViewer)) {

                ProductSequence* sequence = getDbgGuiSharedData().productSequence;
                if (sequence) {
                    al::Scene* scene = sequence->tryGetCurrentScene();
                    if (scene) {
                        al::LiveActorKit* kit = scene->mLiveActorKit;
                        if (kit) {
                            al::LiveActorGroup* allActors = kit->mAllActors;
                            if (allActors) {
                                PlayerActor* player = static_cast<PlayerActor*>(al::getPlayerActor(allActors->mActors[0], 0));
                                PlayerModel* model = player->mPlayerAnimator->mModelHolder->getCurrentModel();
                                ImGui::DragFloat3("Trans", al::getTransPtr(model)->e.data());
                                ImGui::DragFloat3("Rotate", al::getRotatePtr(model)->e.data());
                                ImGui::DragFloat3("Scale", al::getScalePtr(model)->e.data());
                                ImGui::DragFloat3("Gravity", al::getGravityPtr(model)->e.data());
                                ImGui::Text("Figure: %s", pe::util::getPowerUpName(player->mPlayer->mFigureDirector->mCurFigure));
                                ImGui::Text("Name: %s", model->getName());
                                ImGui::Text("Anim Rate: %f", al::getSklAnimFrameRate(model, 0));
                                float* bw = player->mPlayerAnimator->mBlendWeights;
                                ImGui::Text("Blend Weights: %.2f %.2f %.2f %.2f %.2f %.2f", bw[0], bw[1], bw[2], bw[3], bw[4], bw[5]);
                                ImGui::Text("Anim: %s", player->mPlayerAnimator->mCurAnimName.cstr());
                                ImGui::Text("SubAnim: %s", player->mPlayerAnimator->mCurSubAnimName.cstr());

                                PlayerAction* action = player->mPlayer->mActionGraph->getCurrentNode()->getAction();

                                if (action) {
                                    const std::pair<uintptr_t, const char*> shit[] {
                                        { 0x01379a28, "PlayerActionWaitOrBattleWait" },
                                        { 0x013799c8, "PlayerActionWait" },
                                        { 0x01379a88, "PlayerActionWall" },
                                        { 0x01379ad8, "PlayerActionWallClimb" },
                                        { 0x01379b48, "PlayerActionWallClimbJump" },
                                        { 0x01379cd0, "PlayerActionWallClimbSlide" },
                                        { 0x01379d40, "PlayerActionWallFall" },
                                        { 0x01379e60, "PlayerActionWallJump" },
                                        { 0x01377e60, "PlayerActionNormalDie" },
                                    };

                                    bool foundAction = false;
                                    for (int i = 0; i < sizeof(shit) / sizeof(shit[0]); i++) {
                                        if (shit[i].first == util::getVftOffsetMain(action)) {
                                            ImGui::Text("PlayerAction: %s", shit[i].second);
                                            foundAction = true;
                                            break;
                                        }
                                    }

                                    if (!foundAction)
                                        ImGui::Text("PlayerAction: 0x%.8lx", util::getVftOffsetMain(action));
                                } else
                                    ImGui::Text("PlayerAction: nullptr");

                                if (ImGui::CollapsingHeader("Scene")) {
                                    const al::Nerve* nrv = scene->getNerveKeeper()->getCurrentNerve();

                                    const std::pair<uintptr_t, const char*> shit[] {
                                        { 0x0138b818, "Restart" },
                                        { 0x0138b908, "Play" },
                                    };

                                    bool foundNrv = false;
                                    for (int i = 0; i < sizeof(shit) / sizeof(shit[0]); i++) {
                                        if (shit[i].first == util::getVftOffsetMain(nrv)) {
                                            ImGui::Text("Nerve: %s", shit[i].second);
                                            foundNrv = true;
                                            break;
                                        }
                                    }

                                    if (!foundNrv)
                                        ImGui::Text("Nerve: 0x%.8lx", util::getVftOffsetMain(nrv));
                                }
                            }
                        }
                    }
                }
            }
            ImGui::End();
        }
    }

} // namespace gui
} // namespace pe
