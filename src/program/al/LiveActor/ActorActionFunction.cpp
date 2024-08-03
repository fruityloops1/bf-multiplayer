#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/LiveActor.h"

namespace al {

void bindSklAnimRetargetting(const LiveActor* actor, const SklAnimRetargettingInfo* info)
{
    actor->mModelKeeper->mModelCafe->mAnimPlayerSkl->mRetargettingInfo = info;
}

} // namespace al