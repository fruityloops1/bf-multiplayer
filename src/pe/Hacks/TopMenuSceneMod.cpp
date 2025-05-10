#include "pe/Hacks/TopMenuSceneMod.h"
#include "Game/Layout/ButtonGroup.h"
#include "Game/Layout/RCSControlGuideBar.h"
#include "Game/Layout/TopMenu.h"
#include "Game/Scene/TopMenuScene.h"
#include "Game/System/AssetLoadingThread.h"
#include "Game/System/GameDataHolder.h"
#include "Game/System/SaveDataAccessFunction.h"
#include "al/Base/String.h"
#include "al/Controller/ControllerUtil.h"
#include "al/Functor/FunctorV0M.h"
#include "al/Layout/LayoutActor.h"
#include "al/Layout/LayoutInitInfo.h"
#include "al/LiveActor/ActorInitFunction.h"
#include "al/Nerve/NerveFunction.h"
#include "al/Npc/WipeSimple.h"
#include "al/Scene/SceneInitInfo.h"
#include "al/Se/SeFunction.h"
#include "al/System/SaveData.h"
#include "al/Thread/AsyncFunctorThread.h"
#include "heap/seadHeapMgr.h"
#include "helpers/fsHelper.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "imgui.h"
#include "nn/socket.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Enet/NetClient.h"
#include "pe/Hacks/RCSPlayers.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Multiplayer/SaveDataSync.h"
#include "pe/Util/Log.h"
#include "pe/Util/Math.h"
#include "pe/Util/Nerve.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/SoftwareKeyboard.h"
#include "pe/Util/Type.h"
#include "prim/seadSafeString.h"
#include "prim/seadStringUtil.h"
#include <cstdlib>
#include <limits>

static pe::util::SoftwareKeyboard* sKeyboard = nullptr;

TopMenu::TopMenu(TopMenuScene* scene, const al::LayoutInitInfo& info, GameDataHolder* holder, StageWipeKeeper* wipeKeeper, const char* something)
    : LayoutActor("TopMenu")
    , mGameDataHolder(holder)
    , mWipeKeeper(wipeKeeper)
    , mScene(scene) {
    al::initLayoutActor(this, info, "RCS_TopMenu", nullptr);
    initNerve(pe::util::getNerveAt(0x012f3780 /* Appear */));
    mNewGameResumeButtonGroup = new ButtonGroup(info, this, "RCS_TopMenu", "SingleModeTitle", false);
}

namespace pe {

    static void layoutConstructor(TopMenuExt* thisPtr, TopMenuScene* scene, const al::LayoutInitInfo& info, GameDataHolder* holder, StageWipeKeeper* wipeKeeper, const char* something) {
        new (thisPtr) TopMenuExt(scene, info, holder, wipeKeeper, something);
    }

    namespace {
        NERVE_DEF(TopMenuExt, CharSelect)
        NERVE_DEF(TopMenuExt, CharSelectBack)
        NERVE_DEF(TopMenuExt, ModeSelect)
        NERVE_DEF(TopMenuExt, ModeSelectOut)
        NERVE_DEF(TopMenuExt, EnterIp)
        NERVE_DEF(TopMenuExt, Connect)
        NERVE_DEF(TopMenuExt, DownloadSave)
        NERVE_DEF(TopMenuExt, ConnectEnd)
    }

    TopMenuExt::TopMenuExt(TopMenuScene* scene, const al::LayoutInitInfo& info, GameDataHolder* holder, StageWipeKeeper* wipeKeeper, const char* something)
        : TopMenu(scene, info, holder, wipeKeeper, something) {
        mCharSelectButtonGroup = new ButtonGroup(info, this, "RCS_TopMenu", "SingleModeCharSelect", false);
        al::setPaneString(mCharSelectButtonGroup->getButton("Mario"), "TxtButton", u"Mario");
        al::setPaneString(mCharSelectButtonGroup->getButton("Luigi"), "TxtButton", u"Luigi");
        al::setPaneString(mCharSelectButtonGroup->getButton("Peach"), "TxtButton", u"Peach");
        al::setPaneString(mCharSelectButtonGroup->getButton("Kinopio"), "TxtButton", u"Toad");
        al::setPaneString(mCharSelectButtonGroup->getButton("Rosetta"), "TxtButton", u"Rosalina");
        animateCharSelectIn(0);
        al::hidePane(this, "TxtNetworkConnectStatus");

        mModeSelectButtonGroup = new ButtonGroup(info, this, "RCS_TopMenu", "SingleModeModeSelect", false);
        al::setPaneString(mModeSelectButtonGroup->getButton("SinglePlayer"), "TxtButton", u"Singleplayer");
        al::setPaneString(mModeSelectButtonGroup->getButton("MultiPlayer"), "TxtButton", u"Connect to Server");
    }

    void TopMenuExt::animateCharSelectIn(float frames) {
        float outAmount = pe::easeInSine(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 200;
        al::setPaneLocalTrans(mNewGameResumeButtonGroup->getButton("Resume"), "Button", { 0, -outAmount });
        al::setPaneLocalTrans(mNewGameResumeButtonGroup->getButton("NewGame"), "Button", { 0, -outAmount });
        float inAmount = 400 - pe::easeOutQuad(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 500;
        al::setPaneLocalTrans(this, "ButtonSetCharSelect", { 0, -inAmount });
    }

    void TopMenuExt::animateCharSelectOut(float frames) {
        float outAmount = 200 - pe::easeOutQuad(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 200;
        al::setPaneLocalTrans(mNewGameResumeButtonGroup->getButton("Resume"), "Button", { 0, -outAmount });
        al::setPaneLocalTrans(mNewGameResumeButtonGroup->getButton("NewGame"), "Button", { 0, -outAmount });
        float inAmount = pe::easeInSine(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 500 - 100;
        al::setPaneLocalTrans(this, "ButtonSetCharSelect", { 0, -inAmount });
    }

    void TopMenuExt::animateModeSelectIn(float frames) {
        if (mNerveBackCharSelect == util::getNerveAt(0x012f3758) /* SingleModeTitle */) {
            float outAmount = 14 + pe::easeInSine(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 200;
            al::setPaneLocalTrans(this, "ButtonSet2", { 0, -outAmount });
        }

        float inAmount = 500 - pe::easeOutQuad(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 486;
        al::setPaneLocalTrans(this, "ButtonSet3", { 0, -inAmount });
    }

    void TopMenuExt::animateModeSelectOut(float frames) {
        if (mNextNerveAfterModeSelect == util::getNerveAt(0x012f3758 /* SingleModeTitle */)) {
            float outAmount = 500 - pe::easeOutQuad(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 486;
            al::setPaneLocalTrans(this, "ButtonSet2", { 0, -outAmount });
        }

        // float inAmount = 400 - pe::easeOutQuad(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 500;
        // al::setPaneLocalTrans(this, "ButtonSetCharSelect", { 0, -inAmount });

        float inAmount = pe::easeInSine(sead::Mathf::clamp(frames / sCharSelectAndModeSelectAnimFrames, 0, 1)) * 500 + 14;
        al::setPaneLocalTrans(this, "ButtonSet3", { 0, -inAmount });
    }

    void TopMenuExt::exeCharSelect() {
        if (al::isFirstStep(this)) {
            mNewGameResumeButtonGroup->invalidate();
            mCharSelectButtonGroup->reset();
        }

        if (al::isStep(this, sCharSelectAndModeSelectAnimFrames)) {
            mCharSelectButtonGroup->validate();
            mCharSelectButtonGroup->select(util::getPlayerName(RCSPlayers::getCurrentPlayerType()));
            mCharSelectButtonGroup->showCursorAppear();
            mNewGameResumeButtonGroup->reset();
        }

        mCharSelectButtonGroup->updateAndCursorDefault(-1);

        animateCharSelectIn(mIsReturnFromFileSelect ? sCharSelectAndModeSelectAnimFrames : al::getNerveStep(this));

        if (al::isPadTriggerB(-1)) {
            al::setNerve(this, &nrvTopMenuExtCharSelectBack);
            mCharSelectButtonGroup->invalidate();
            mCharSelectButtonGroup->hideCursor();

            al::startSe(this, "Back");
        } else if (mCharSelectButtonGroup->isDecideAny()) {
            const char* name = mCharSelectButtonGroup->getSelectedButtonName();
            RCSPlayers::setPlayerType(util::getPlayerType(name));
            al::setNerve(this, mNextNerveAfterCharSelect);
            mCharSelectButtonGroup->invalidate();
            mCharSelectButtonGroup->hideCursor();

            al::startSe(this, "Select");
        }
    }

    void TopMenuExt::exeCharSelectBack() {
        animateCharSelectOut(al::getNerveStep(this));

        if (al::isStep(this, sCharSelectAndModeSelectAnimFrames)) {
            al::setNerve(this, mNerveBackCharSelect);
        }
    }

    void TopMenuExt::exeModeSelect() {
        if (al::isFirstStep(this)) {

            mNewGameResumeButtonGroup->invalidate();
            mModeSelectButtonGroup->reset();
        }

        if (al::isStep(this, sCharSelectAndModeSelectAnimFrames)) {
            mModeSelectButtonGroup->validate();
            mModeSelectButtonGroup->select("SinglePlayer");
            mModeSelectButtonGroup->showCursorAppear();
            mNewGameResumeButtonGroup->reset();
        }

        animateModeSelectIn(mIsModeSelectFromBootup ? sCharSelectAndModeSelectAnimFrames : al::getNerveStep(this));

        mModeSelectButtonGroup->updateAndCursorDefault(-1);

        if (mModeSelectButtonGroup->isDecideAny()) {
            const char* name = mModeSelectButtonGroup->getSelectedButtonName();

            mModeSelectButtonGroup->invalidate();
            mModeSelectButtonGroup->hideCursor();

            if (al::isEqualString(name, "SinglePlayer")) {
                al::showPane(this, "ButtonSet2");

                mNextNerveAfterModeSelect = util::getNerveAt(0x012f3758) /* SingleModeTitle */;
                mNerveBackCharSelect = util::getNerveAt(0x012f3758 /* SingleModeTitle */);
            } else {
                mNextNerveAfterCharSelect = &nrvTopMenuExtEnterIp;
                mNextNerveAfterModeSelect = &nrvTopMenuExtCharSelect;
                mNerveBackCharSelect = &nrvTopMenuExtModeSelect;
                mIsModeSelectFromBootup = false;
            }

            al::setNerve(this, &nrvTopMenuExtModeSelectOut);
            al::startSe(this, "Select");
        }
    }

    void TopMenuExt::exeModeSelectOut() {
        animateModeSelectOut(al::getNerveStep(this));
        if (al::isStep(this, sCharSelectAndModeSelectAnimFrames))
            al::setNerve(this, mNextNerveAfterModeSelect);
    }

    // shit dont work
    /* static nn::swkbd::TextCheckResult textCheckCallback(void* errorTextBuf, unsigned long* errorTextLength, nn::swkbd::String* huh)
    {
        char balls[128];
        sead::StringUtil::convertUtf16ToUtf8(balls, 128, huh->strBuf, 128);
        pe::log("cock %s", balls);

        memcpy(errorTextBuf, u"balls", sizeof(u"balls"));
        *errorTextLength = sizeof(u"balls");
        return nn::swkbd::TextCheckResult::ShowFailureDialog;
    }

    // nn::swkbd::SetTextCheckCallback(&arg, textCheckCallback);
    */

    void TopMenuExt::showEnterIpSwkbd() {
        nn::swkbd::ShowKeyboardArg arg;
        nn::swkbd::MakePreset(&arg.keyboardConfig, nn::swkbd::Preset::Default);
        arg.keyboardConfig.keyboardMode = nn::swkbd::KeyboardMode::ModeNumeric;
        arg.keyboardConfig.textMaxLength = 21;
        arg.keyboardConfig.textMinLength = 7;
        arg.keyboardConfig.inputFormMode = nn::swkbd::InputFormMode::OneLine;
        nn::swkbd::SetLeftOptionalSymbolKey(&arg.keyboardConfig, u':');
        nn::swkbd::SetRightOptionalSymbolKey(&arg.keyboardConfig, u'.');

        sKeyboard->show(arg, mKeyboardHeaderText.cstr(), mKeyboardSubText.cstr(), "(for example: 192.168.178.109:7089)", "OK", TopMenuExt::enterIpCallback, this, "");
    }

    void TopMenuExt::exeEnterIp() {
        if (al::isFirstStep(this)) {
            sead::ScopedCurrentHeapSetter setter(pe::gui::getPeepaHeap());
            if (sKeyboard == nullptr)
                sKeyboard = new util::SoftwareKeyboard;

            al::setPaneLocalTrans(this, "ButtonSetCharSelect", { 0, -400 });

            mKeyboardHeaderText = "Please enter the IP address of the server to connect to";
            mKeyboardSubText = "Adding a port is optional. Default port: 7089";
            showEnterIpSwkbd();
        }
    }

    void TopMenuExt::exeConnect() {
        if (al::isFirstStep(this)) {
            mConnectionAttemptTimerStartTick = nn::os::GetSystemTick();
            al::showPane(this, "TxtNetworkConnectStatus");
        }

        if (!enet::getNetClient()->isConnected() && al::isPadTriggerB()) {
            enet::getNetClient()->disconnect();
            al::hidePane(this, "TxtNetworkConnectStatus");
            al::setNerve(this, &nrvTopMenuExtModeSelect);
            mIsModeSelectFromBootup = false;
            al::startSe(this, "Back");
        }

        char msg[128];
        enet::getNetClient()->printStatusMsgPretty(msg, float(nn::os::GetSystemTick().m_tick - mConnectionAttemptTimerStartTick) / nn::os::GetSystemTickFrequency());
        char16_t msgWide[128];
        sead::StringUtil::convertUtf8ToUtf16(msgWide, 128, msg, 128);

        al::setPaneString(this, "TxtNetworkConnectStatus", msgWide);

        if (enet::getNetClient()->isConnected()) {
            getSaveDataMode() = SaveDataMode::Server;

            al::setNerve(this, &nrvTopMenuExtDownloadSave);
        }
    }

    void TopMenuExt::exeDownloadSave() {
        if (al::isFirstStep(this)) {
            resetSaveDataBuffer();
        }

        if (!enet::getNetClient()->isConnected()) {
            enet::getNetClient()->disconnect();
            al::hidePane(this, "TxtNetworkConnectStatus");
            al::setNerve(this, &nrvTopMenuExtModeSelect);
            mIsModeSelectFromBootup = false;
            al::startSe(this, "Back");

            resetSaveDataBuffer();
        }

        char progress[8] { '-' };

        bool isComplete = true;
        for (int i = 0; i < 8; i++) {
            progress[i] = getSaveDataAckBuffer()[i] ? '#' : '-';
            if (getSaveDataAckBuffer()[i] == false) {
                isComplete = false;
            }
        }

        char msg[128];
        snprintf(msg, 128, "Downloading Save... %s", progress);
        char16_t msgWide[128];
        sead::StringUtil::convertUtf8ToUtf16(msgWide, 128, msg, 128);

        al::setPaneString(this, "TxtNetworkConnectStatus", msgWide);

        if (isComplete) {
            FsHelper::writeFileToPath((void*)getSaveDataWorkBuffer(), getSaveDataWorkBufferSize(), "sd:/BowsersFuryOnline/cache/GameDataRemote.bin");
            al::setNerve(this, &nrvTopMenuExtConnectEnd);
        }
    }

    void TopMenuExt::exeConnectEnd() {
    }

    bool TopMenuExt::isConnectEnd() const { return al::isNerve(this, &nrvTopMenuExtConnectEnd); }

    void TopMenuExt::enterIpCallback(bool closed, const nn::swkbd::String& str) {
        if (closed) {
            al::setNerve(this, &nrvTopMenuExtModeSelect);
            return;
        }

        sead::FixedSafeString<22> host;
        sead::FixedSafeString<16> ip;
        u16 port = 7089;
        bool invalid = false;

        sead::StringUtil::convertUtf16ToUtf8(host.getBuffer(), 22, str.strBuf, 22);

        s32 semicolonIdx = host.findIndex(":");
        if (semicolonIdx == -1) {
            ip.copy(host);
        } else if (host.calcLength() > semicolonIdx + 1) {
            sead::SafeString portStr = host.getPart(semicolonIdx + 1);

            char* endptr = nullptr;
            long parsedPort = strtol(portStr.cstr(), &endptr, 10);

            if (endptr != portStr.cstr() && *endptr == '\0' && parsedPort >= std::numeric_limits<u16>::min() && parsedPort <= std::numeric_limits<u16>::max())
                port = parsedPort;
            else
                invalid = true;
            ip.copy(host, semicolonIdx);
        } else
            invalid = true;

        // TODO: CHECK IP VALIDITY
        // im too lazy

        if (invalid) {
            mKeyboardSubText.format("Error: '%s' is not a valid hostname! Please try again", host.cstr());
            showEnterIpSwkbd();
        } else {
            enet::getNetClient()->connect(ip.cstr(), port);
            al::setNerve(this, &nrvTopMenuExtConnect);
        }
    }

    NERVE_DEF(TopMenuSceneExt, Warning);
    NERVE_DEF(TopMenuSceneExt, LoadNetworkSave);

    static bool sAlreadyEntered = false;
    static bool sAlreadyPressedA = false;

    void TopMenuSceneExt::exeWarning() {
        if (al::isFirstStep(this)) {
            mWarningLayout->appear();
            mControlGuideBar->appearWithMessage(RCSControlGuideBar::GuideBarMsgType::Confirm, -1);

            // just start the loading thread now instead of when the user selects a save, since we'll never load 3d world anyway
            // rc::AssetLoadingThread::instance()->startLoad(rc::AssetLoadingThread::LOAD_TYPE::SingleMode);
            // except we don't because it makes the menu laggy and it's a negligible amount of time save
        }

        if (!sAlreadyPressedA && al::isPadTriggerA()) {
            mBlackWipe->appear();
            mBlackWipe->startClose(-1);
            mControlGuideBar->end();

            sAlreadyPressedA = true;
        }

        if (mBlackWipe->isCloseEnd() or sAlreadyEntered) {
            if (!sAlreadyEntered)
                mBlackWipe->startOpen(-1);

            appear(TopMenuAppearState::SingleMode, false);
            sAlreadyEntered = true;
        }
    }

    void TopMenuSceneExt::exeLoadNetworkSave() {
        if (al::isFirstStep(this)) {
            al::requestReadSaveData("GameData.bin", 0x10000, 0);
            mGameDataHolder->setSingleModePlayingFileID(3, false);
        }

        if (al::updateSaveDataSequence()) {
            mGameDataHolder->readFromSaveDataBuffer();
            al::stopSequenceBgm(this, "SingleModeTitle", 60);
            al::tryStopSe(this, "Storm");
            al::startSe(this, "GameStart");
            al::setNerve(this, util::getNerveAt(0x0138cca0) /* TopMenuEnd */);
        }
    }

    TopMenuSceneExt::~TopMenuSceneExt() {
    }

    struct Hooks {
        static void setWarningNrv(al::IUseNerve* user) {
            al::setNerve(user, &nrvTopMenuSceneExtWarning);
        }

        static void setModeSelectNerve(TopMenuExt* thisPtr, const al::Nerve* nrv) {
            thisPtr->mIsModeSelectFromBootup = true;
            al::setNerve(thisPtr, &nrvTopMenuExtModeSelect);
        }

        static void setCharSelectNerve(TopMenuExt* thisPtr, const al::Nerve* nrv) {
            thisPtr->mNextNerveAfterCharSelect = nrv;
            thisPtr->mIsReturnFromFileSelect = false;
            al::setNerve(thisPtr, &nrvTopMenuExtCharSelect);
        }

        static void setCharSelectNerveFromEmptySave(TopMenuExt* thisPtr, const al::Nerve* nrv) {
            setCharSelectNerve(thisPtr, nrv);
        }

        static void setCharSelectNerveReturnFromFileSelect(TopMenuExt* thisPtr, const al::Nerve* nrv) {
            thisPtr->mIsReturnFromFileSelect = true;
            al::setNerve(thisPtr, &nrvTopMenuExtCharSelect);
        }

        static bool firstStepAppearSingleModeButtons(al::IUseNerve* user) {
            bool isFirstStep = al::isFirstStep(user);

            if (isFirstStep) {
                TopMenuExt* menu = static_cast<TopMenuExt*>(user);
                al::hidePane(menu, "ButtonSet2");
            }

            return isFirstStep;
        }

        static bool onSingleModeTitleLeave(int controllerPort) {
            TopMenuExt* thisPtr;
            __asm("mov %[lyt], x19" : [lyt] "=r"(thisPtr));

            bool isLeave = al::isPadTriggerB(controllerPort);
            if (isLeave) {
                thisPtr->mIsModeSelectFromBootup = false;
                al::setNerve(thisPtr, &nrvTopMenuExtModeSelect);
                al::startSe(thisPtr, "Back");
            }
            return false;
        }
    };

    HkTrampoline<void, TopMenuSceneExt*> topMenuSceneSingleModeTitleHook = hk::hook::trampoline([](TopMenuSceneExt* thisPtr) {
        if (getSaveDataMode() == SaveDataMode::Local)
            topMenuSceneSingleModeTitleHook.orig(thisPtr);
        else {
            TopMenuExt* menu = static_cast<TopMenuExt*>(thisPtr->mTopMenuLayout);
            if (menu->isConnectEnd()) {
                al::setNerve(thisPtr, &nrvTopMenuSceneExtLoadNetworkSave);
            }
        }
    });

    HkTrampoline<void, TopMenuSceneExt*, TopMenuAppearState, bool> topMenuSceneAppearHook = hk::hook::trampoline([](TopMenuSceneExt* thisPtr, TopMenuAppearState state, bool thing) -> void {
        topMenuSceneAppearHook.orig(thisPtr, state, thing);

        getSaveDataMode() = SaveDataMode::Local;
        enet::getNetClient()->disconnect();
    });

    void initLayoutHook(al::LayoutInitInfo* info, TopMenuSceneExt* scene, const al::SceneInitInfo& sceneInfo) {
        al::initLayoutInitInfo(info, scene, sceneInfo);

        scene->mWarningLayout = new al::LayoutActor("Warning Layout");
        al::initLayoutActor(scene->mWarningLayout, *info, "PeWarningLayout1");
        scene->mWarningLayout->kill();

        scene->mBlackWipe = new al::WipeSimple("Warning Wipe", "WipeFadeBlack", *info);
        scene->mBlackWipe->kill();
    }

    void installTopMenuSceneMod() {
        int shit = sizeof(TopMenuExt);
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003fad48, initLayoutHook);
        hk::ro::getMainModule()->writeRo(0x00404acc, 0x52802900); // mov w0, sizeof(TopMenuSceneExt)
        hk::ro::getMainModule()->writeRo(0x003fad4c, 0x52807A00); // mov w0, sizeof(TopMenuExt)
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003fad6c, layoutConstructor);

        hk::hook::writeBranch(hk::ro::getMainModule(), 0x003fb350, Hooks::setWarningNrv);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x003fb368, Hooks::setWarningNrv);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x003fbb8c, Hooks::setWarningNrv);
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0019f17c, Hooks::setCharSelectNerve);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x0019f4e0, Hooks::setCharSelectNerveReturnFromFileSelect);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x0019f0b0, Hooks::setCharSelectNerveFromEmptySave);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x0019eebc, Hooks::setModeSelectNerve);
        hk::ro::getMainModule()->writeRo(0x0019f07c, 0xD503201F);
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0019ee40, Hooks::firstStepAppearSingleModeButtons);
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x0019f0b8, Hooks::onSingleModeTitleLeave);

        topMenuSceneAppearHook.installAtOffset(hk::ro::getMainModule(), 0x003fae60);
        topMenuSceneSingleModeTitleHook.installAtOffset(hk::ro::getMainModule(), 0x003fb4f0);

        // remove back button hint (softlock fail)
        // Patcher(0x0019ee08).Write(exl::armv8::inst::Nop());
        // Patcher(0x0019ee00).Write(exl::armv8::inst::Nop());
    }

} // namespace pe
