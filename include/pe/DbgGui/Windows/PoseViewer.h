#pragma once

#include "pe/DbgGui/IComponent.h"

namespace pe {
namespace gui {

    class PoseViewer : public IComponent {
    public:
        PoseViewer();

        void update() override;
        void draw() override;
    };

} // namespace gui
} // namespace pe
