#pragma once

#include "Game/Sequence/ProductSequence.h"
#include "container/seadPtrArray.h"
#include <sead/heap/seadDisposer.h>
#include <sead/heap/seadExpHeap.h>

struct ProductSequenceInitHook;

namespace pe {
namespace gui {

    class IComponent;
    class Peekabo;

    sead::ExpHeap*& getPeepaHeap();

    class DbgGui {
        SEAD_SINGLETON_DISPOSER(DbgGui);

        sead::PtrArray<IComponent> mComponents;

        struct {
            ProductSequence* productSequence = nullptr;

            bool showDemoWindow = false;
            bool showLog = true;
            bool showHeapViewer = false;
            bool showActorBrowser = false;
            bool showHacks = false;
            bool showRCSCamera = false;
            bool showMultiplayer = false;
            bool showPoseViewer = false;
        } mSharedData;

        bool mIsSingleModeScene = false;

    public:
        DbgGui();

        void update();
        void draw();

        bool isSingleModeScene() const { return mIsSingleModeScene; }

        auto& getSharedData() { return mSharedData; }

        friend class IComponent;
        friend ::ProductSequenceInitHook;
    };

    void initDbgGuiHooks();

} // namespace gui
} // namespace pe
