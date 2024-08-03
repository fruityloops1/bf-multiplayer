#pragma once

#include "al/LiveActor/LiveActor.h"

class PlayerModel : public al::LiveActor {
    void* unk1;

public:
    const char* mPlayerName;
    const char* mFigureName;
    void* _160;
    void* _168;
    void* _170;
    void* _178;
    void* _180;
    void* _188;
    class PlayerModelIK* mModelIK;
    class PlayerModelHair* mModelHair;
    int _1a0;
    int _1a4;
    void* _1a8;
    u8 unk2[0x2a];

    PlayerModel(const char* name, const char* archiveName, const al::ActorInitInfo& info, const char* subArchiveName);
};