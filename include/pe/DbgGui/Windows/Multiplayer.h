#pragma once

#include "pe/DbgGui/IComponent.h"

namespace pe {
class MultiplayerManager;
namespace gui {

    class Multiplayer : public IComponent {
        MultiplayerManager* mMgr = nullptr;

    public:
        void update() override;
        void draw() override;
    };

} // namespace gui
} // namespace pe
