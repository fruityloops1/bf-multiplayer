#pragma once

#include "pe/DbgGui/DbgGui.h"

namespace pe {
namespace gui {

    class IComponent {
    public:
        virtual void update() = 0;
        virtual void draw() = 0;

        static inline auto& getDbgGuiSharedData() { return DbgGui::instance()->mSharedData; }
    };

} // namespace gui
} // namespace pe
