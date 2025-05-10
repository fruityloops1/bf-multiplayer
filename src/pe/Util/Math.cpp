#include "pe/Util/Math.h"
#include "math/seadMatrix.h"
#include "math/seadVectorFwd.h"

namespace pe {

sead::Vector3f quatToRotate(const sead::Quatf& quat)
{
    sead::Vector3f rotate;
    quat.calcRPY(rotate);
    rotate *= Constants<float>::radiansToDegrees;
    return rotate;
}

} // namespace pe