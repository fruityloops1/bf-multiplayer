#pragma once

#include "Game/Layout/ButtonGroup.h"
#include "Game/Layout/TopMenu.h"
#include "Game/Scene/TopMenuScene.h"
#include "Game/Sequence/StageWipeKeeper.h"
#include "Game/System/GameDataHolder.h"
#include "al/Functor/FunctorV0M.h"
#include "al/Layout/LayoutInitInfo.h"
#include "al/Npc/WipeSimple.h"
#include "al/Thread/AsyncFunctorThread.h"
#include "pe/Util/SoftwareKeyboard.h"
#include <sead/prim/seadSafeString.h>

namespace pe {

void installTopMenuSceneMod();

class TopMenuExt : public TopMenu {
    ButtonGroup* mCharSelectButtonGroup = nullptr;
    ButtonGroup* mModeSelectButtonGroup = nullptr;
    bool mIsReturnFromFileSelect = false;
    const al::Nerve* mNextNerveAfterCharSelect = nullptr;
    const al::Nerve* mNerveBackCharSelect = nullptr;
    bool mIsModeSelectFromBootup = false;
    const al::Nerve* mNextNerveAfterModeSelect = nullptr;
    sead::FixedSafeString<256> mKeyboardHeaderText;
    sead::FixedSafeString<256> mKeyboardSubText;
    s64 mConnectionAttemptTimerStartTick = 0;

    void animateCharSelectIn(float frames);
    void animateCharSelectOut(float frames);
    void animateModeSelectIn(float frames);
    void animateModeSelectOut(float frames);

    static constexpr int sCharSelectAndModeSelectAnimFrames = 15;

public:
    TopMenuExt(TopMenuScene* scene, const al::LayoutInitInfo& info, GameDataHolder* holder, StageWipeKeeper* wipeKeeper, const char* something);

    void exeCharSelect();
    void exeCharSelectBack();
    void exeModeSelect();
    void exeModeSelectOut();
    void exeEnterIp();
    void exeConnect();
    void exeDownloadSave();
    void exeConnectEnd();

    void enterIpCallback(bool closed, const nn::swkbd::String& str);
    void showEnterIpSwkbd();

    bool isConnectEnd() const;

    static void enterIpCallback(void* userData, bool closed, const nn::swkbd::String& str)
    {
        static_cast<TopMenuExt*>(userData)->enterIpCallback(closed, str);
    }

    friend struct Hooks;
    friend class TopMenuSceneExt;
};

class TopMenuSceneExt : public TopMenuScene {
public:
    al::LayoutActor* mWarningLayout = nullptr;
    al::WipeSimple* mBlackWipe = nullptr;

    void exeWarning();
    void exeLoadNetworkSave();
    ~TopMenuSceneExt() override;
};

} // namespace pe
