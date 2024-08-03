#pragma once

#include "Game/Player/PlayerActor.h"
#include "Game/Player/PlayerModelHolder.h"
#include "al/Debug/HioNode.h"

class PlayerModelHolderCreator {
public:
    struct CreationFlag {
        bool createIK : 1;
        bool createHairCtrl : 1;
        bool createSkirtDynamics : 1;
        bool createTailJointController : 1;
        bool createHairJointController : 1;

        operator u32() { return *reinterpret_cast<u8*>(this); }
    };

    static PlayerModelHolder* createModelHolderAll(const al::ActorInitInfo& info, const char* playerName, IUsePlayerRetargettingInfoCreator*, al::IUseHioNode*, bool createFur, u32 creationFlag, const char* archiveSuffix, int);
};
