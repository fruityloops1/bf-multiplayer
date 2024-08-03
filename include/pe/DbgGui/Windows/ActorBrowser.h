#pragma once

#include "Game/Player/PlayerActor.h"
#include "Game/Sequence/ProductSequence.h"
#include "al/LiveActor/LiveActor.h"
#include "pe/DbgGui/IComponent.h"

namespace pe {
namespace gui {

    class ActorBrowser : public IComponent {
        al::LiveActor* mCurrentSelection = nullptr;

        void showActorView();
        void showBrowser(ProductSequence* sequence);

        void showActorInList(al::LiveActor* actor);
        void showActorInListNoUnitConfigName(al::LiveActor* actor);
        void showPlayerActor(PlayerActor* actor);

        void showActorPoseKeeper();
        void showNerveKeeper();
        void showSubActorKeeper();
        void showLiveActorFlag();

    public:
        void update() override;
        void draw() override;
    };

} // namespace gui
} // namespace pe
