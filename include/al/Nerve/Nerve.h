#pragma once

namespace al {

class NerveKeeper;
class Nerve {
    virtual void execute(NerveKeeper*) const = 0;
    virtual void executeOnEnd(NerveKeeper*) const {};
};

#define NERVE_DEF(CLASS, ACTION)                                   \
    struct CLASS##Nrv##ACTION : public ::al::Nerve {               \
        virtual void execute(::al::NerveKeeper* keeper) const      \
        {                                                          \
            static_cast<CLASS*>(keeper->getHost())->exe##ACTION(); \
        }                                                          \
    };                                                             \
    const CLASS##Nrv##ACTION nrv##CLASS##ACTION = CLASS##Nrv##ACTION();

#define NERVE_DEF_END(CLASS, ACTION, ENDACTION)                       \
    struct CLASS##Nrv##ACTION : public ::al::Nerve {                  \
        virtual void execute(::al::NerveKeeper* keeper) const         \
        {                                                             \
            static_cast<CLASS*>(keeper->getHost())->exe##ACTION();    \
        }                                                             \
        virtual void executeOnEnd(::al::NerveKeeper* keeper) const    \
        {                                                             \
            static_cast<CLASS*>(keeper->getHost())->exe##ENDACTION(); \
        }                                                             \
    };                                                                \
    const CLASS##Nrv##ACTION nrv##CLASS##ACTION = CLASS##Nrv##ACTION();

} // namespace al
