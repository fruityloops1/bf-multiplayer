#include "pe/MapObj/PatanPanelStarter.h"
#include "al/Functor/FunctorV0M.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/LiveActor/ActorClippingFunction.h"
#include "al/Nerve/Nerve.h"
#include "al/Nerve/NerveFunction.h"

namespace pe {

namespace {
    NERVE_DEF(PatanPanelStarter, OffWait);
    NERVE_DEF(PatanPanelStarter, OnWait);
    NERVE_DEF(PatanPanelStarter, ToOff);
    NERVE_DEF(PatanPanelStarter, ToOn);
}

void PatanPanelStarter::init(const al::ActorInitInfo& info)
{
    al::initActor(this, info);
    al::initNerve(this, &nrvPatanPanelStarterOffWait);

    al::killPrePassLightAll(this, 10);
    al::appearPrePassLight(this, "Off光源", -1);
    al::listenStageSwitchOff(this, "StartSwitchOn", al::FunctorV0M<PatanPanelStarter*, void (PatanPanelStarter::*)()>(this, &PatanPanelStarter::close));
    makeActorAppeared();
}

void PatanPanelStarter::exeOffWait()
{
    if (al::isFirstStep(this)) {
        al::validateClipping(this);
        al::startAction(this, "OffWait");
    }
}

void PatanPanelStarter::exeOnWait()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "OnWait");
        al::validateClipping(this);
    }
}

void PatanPanelStarter::exeToOff()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "OnToOff");
        al::killPrePassLightAll(this, 10);
        al::appearPrePassLight(this, "Off光源", -1);
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &nrvPatanPanelStarterOffWait);
}

void PatanPanelStarter::exeToOn()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Reaction");
        al::killPrePassLightAll(this, 10);
        al::appearPrePassLight(this, "On光源", -1);
    }
    if (al::isActionEnd(this)) {
        al::tryOnStageSwitch(this, "StartSwitchOn");
        al::setNerve(this, &nrvPatanPanelStarterOnWait);
    }
}

bool PatanPanelStarter::receiveMsg(const al::SensorMsg* msg, al::HitSensor* source, al::HitSensor* target)
{
    if (al::isMsgPlayerFloorTouch(msg) && al::isNerve(this, &nrvPatanPanelStarterOffWait)) {
        al::invalidateClipping(this);
        al::setNerve(this, &nrvPatanPanelStarterToOn);
        return true;
    }
    return false;
}

void PatanPanelStarter::close()
{
    if (al::isNerve(this, &nrvPatanPanelStarterOnWait)) {
        al::invalidateClipping(this);
        al::setNerve(this, &nrvPatanPanelStarterToOff);
    }
}

} // namespace pe