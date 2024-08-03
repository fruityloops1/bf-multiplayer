#pragma once

#include "pe/DbgGui/IComponent.h"
#include <math/seadVector.h>

namespace pe {
namespace gui {

    class RCSCamera : public IComponent {
        float mWheelMoveVel = 0;
        sead::Vector2f mCameraMoveVel = sead::Vector2f::zero;
        sead::Vector3f mDirection = sead::Vector3f::ex;

        void updateCameraInput();

    public:
        RCSCamera();

        void update() override;
        void draw() override;
    };

} // namespace gui
} // namespace pe
