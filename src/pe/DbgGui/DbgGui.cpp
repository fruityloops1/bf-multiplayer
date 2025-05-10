#include "pe/DbgGui/DbgGui.h"
#include "Game/Sequence/ProductSequence.h"
#include "al/Controller/ControllerUtil.h"
#include "al/Memory/MemorySystem.h"
#include "al/Sequence/SequenceInitInfo.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "hk/gfx/ImGuiBackendNvn.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "imgui.h"
#include "imgui_nvn.h"
#include "nn/fs.h"
#include "pe/DbgGui/MenuBar.h"
#include "pe/DbgGui/Windows/ActorBrowser.h"
#include "pe/DbgGui/Windows/Hacks.h"
#include "pe/DbgGui/Windows/HeapViewer.h"
#include "pe/DbgGui/Windows/ImGuiDemo.h"
#include "pe/DbgGui/Windows/Log.h"
#include "pe/DbgGui/Windows/Multiplayer.h"
#include "pe/DbgGui/Windows/PoseViewer.h"
#include "pe/DbgGui/Windows/RCSCamera.h"
#include "pe/Hacks/FSHacks.h"
#include "pe/Hacks/PlacementHolderMod.h"
#include "pe/Multiplayer/MultiplayerManager.h"
#include "pe/Util/Log.h"
#include <sead/heap/seadHeapMgr.h>

namespace pe {
    namespace gui {

        static void drawDbgGui() {
            sead::ScopedCurrentHeapSetter heapSetter(pe::gui::getPeepaHeap());

            auto* dbgGui = pe::gui::DbgGui::instance();
            if (dbgGui)
                dbgGui->draw();
        }

        static void productSequenceUpdateHook(ProductSequence* sequence) {
            nvnImGui::UpdateInput();

            sequence->al::Sequence::update();

            {
                sead::ScopedCurrentHeapSetter heapSetter(pe::gui::getPeepaHeap());

                auto* dbgGui = pe::gui::DbgGui::instance();
                if (dbgGui)
                    dbgGui->update();
            }
        }

        static void initDbgGui(ProductSequence* sequence) {
            pe::gui::getPeepaHeap() = sead::ExpHeap::create(1024 * 1024 * 3, "PeepaHeap", al::getSequenceHeap(), 8, sead::ExpHeap::cHeapDirection_Forward, false);
            pe::initializeLog(pe::gui::getPeepaHeap());
            pe::createPlacementInfoHeap();
            {
                sead::ScopedCurrentHeapSetter heapSetter(pe::gui::getPeepaHeap());

                pe::gui::DbgGui::createInstance(nullptr);
                pe::gui::DbgGui::instance()->getSharedData().productSequence = sequence;

                pe::MultiplayerManager::createInstance(nullptr);
                pe::MultiplayerManager::instance()->init();
            }

            pe::applyRomFSPatches();

            nvnImGui::InitImGui();
        }

        HkTrampoline<void, ProductSequence*, const al::SequenceInitInfo&> productSequenceInitHook = hk::hook::trampoline([](ProductSequence* sequence, const al::SequenceInitInfo& info) {
            productSequenceInitHook.orig(sequence, info);

            initDbgGui(sequence);
        });

        HkTrampoline<void, sead::FileDeviceMgr*> createFileDeviceMgrHook = hk::hook::trampoline([](sead::FileDeviceMgr* thisPtr) {
            createFileDeviceMgrHook.orig(thisPtr);
            *(bool*)(uintptr_t(thisPtr) + 81) = nn::fs::MountSdCardForDebug("sd").IsFailure();
        });

        SEAD_SINGLETON_DISPOSER_IMPL(DbgGui)

        sead::ExpHeap*& getPeepaHeap() {
            static sead::ExpHeap* heap { nullptr };
            return heap;
        }

        DbgGui::DbgGui() {
#ifdef DEBUGMENU
            mComponents.allocBuffer(16, nullptr);
            mComponents.pushBack(new MenuBar);
            mComponents.pushBack(new ImGuiDemo);
            mComponents.pushBack(new Log);
            mComponents.pushBack(new HeapViewer);
            mComponents.pushBack(new ActorBrowser);
            mComponents.pushBack(new RCSCamera);
            mComponents.pushBack(new Hacks);
            mComponents.pushBack(new Multiplayer);
            mComponents.pushBack(new PoseViewer);
#endif
        }

        void DbgGui::draw() {
            for (IComponent& c : mComponents)
                c.draw();
        }

        void DbgGui::update() {
            for (IComponent& c : mComponents)
                c.update();
        }

        void initDbgGuiHooks() {
#ifdef DEBUGMENU
            nvnImGui::InstallHooks();
            nvnImGui::addDrawFunc(drawDbgGui);
            hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x003fcf68, productSequenceUpdateHook);
#endif
            productSequenceInitHook.installAtOffset(hk::ro::getMainModule(), 0x003fc4d0);
            createFileDeviceMgrHook.installAtOffset(hk::ro::getMainModule(), 0x0070e7e0);
        }

    } // namespace gui
} // namespace pe
