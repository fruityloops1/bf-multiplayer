#pragma once

#include "pe/DbgGui/IComponent.h"
#include <sead/container/seadPtrArray.h>

namespace pe {
namespace gui {

    class Log : public IComponent {
        char mFilter[128] { 0 };

    public:
        void update() override;
        void draw() override;
    };

} // namespace gui
} // namespace pe
