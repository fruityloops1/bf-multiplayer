#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/Npc/SimpleLayoutAppearWaitEnd.h"

class RCS_UserNameplate : public al::SimpleLayoutAppearWaitEnd {
    int _124 = 0;
    int _128 = 0;
    void* _130 = nullptr;
    int _138 = 0;
    int _13C;
    al::LiveActor* mHost = nullptr;
    void* _148 = nullptr;
    const char* _150 = nullptr;
    bool _158;
    bool _159;
    bool mKeepLayoutAlive;
    bool _15B;
    bool _15C;
    bool _15D;
    void* _160;

public:
    RCS_UserNameplate(al::LiveActor* host, const al::LayoutInitInfo& info);

    void init(const char16_t* name, bool, bool);
    /* disappearOnMovement: makes the name yellow, and only stays visible when mHost is standing in place*/
    void init(const char* name, bool disappearOnMovement, bool keepLayoutAlive);
    void appear() override;
    void kill() override;
    void setInDemo(bool, bool);
    void copy(RCS_UserNameplate* other);

    void exeAppear();
    void exeWaitEnd();
    void exeEnd();

    void control() override;
    bool isHostBehindCamera() const;
    void updateShowHideFromCameraDistance(bool);
};
