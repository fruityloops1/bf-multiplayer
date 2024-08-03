#pragma once

#include "pe/DbgGui/IComponent.h"

namespace pe {
namespace gui {

    class Hacks : public IComponent {
    public:
        Hacks();

        void update() override;
        void draw() override;
    };

} // namespace gui
} // namespace pe
