#pragma once

#include "Game/Layout/GuideGameWindow.h"
#include "Game/Layout/SingleModeSceneLayout.h"
#include "Game/MapObj/DisasterModeController.h"
#include "Game/MapObj/KoopaJr.h"
#include "Game/Player/PlayerRetargettingSelectorSceneObj.h"
#include "al/Scene/ISceneObj.h"
#include "al/Scene/SceneObjHolder.h"

namespace rc {

enum class SceneObjType : int {
    PlayerRetargettingSelectorSceneObj = 25,
    GuideGameWindow = 28,
    SingleModeSceneLayout = 53,
    KoopaJr = 56,
    DisasterModeController = 58
};

inline PlayerRetargettingSelectorSceneObj* getPlayerRetargettingSelector(al::IUseSceneObjHolder* holder) { return static_cast<PlayerRetargettingSelectorSceneObj*>(al::getSceneObj(holder, (int)SceneObjType::PlayerRetargettingSelectorSceneObj)); }
inline GuideGameWindow* getGuideGameWindow(al::IUseSceneObjHolder* holder) { return static_cast<GuideGameWindow*>(al::getSceneObj(holder, (int)SceneObjType::GuideGameWindow)); }
inline KoopaJr* getKoopaJr(al::IUseSceneObjHolder* holder) { return static_cast<KoopaJr*>(al::getSceneObj(holder, (int)SceneObjType::KoopaJr)); }
inline SingleModeSceneLayout* getSingleModeSceneLayout(al::IUseSceneObjHolder* holder) { return static_cast<SingleModeSceneLayout*>(al::getSceneObj(holder, (int)SceneObjType::SingleModeSceneLayout)); };

} // namespace rc