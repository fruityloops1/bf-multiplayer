#pragma once

#include "al/Audio/AudioDirector.h"
#include "al/Model/ModelKeeper.h"
#include <sead/math/seadMatrix.h>
#include <sead/math/seadVectorFwd.h>

namespace al {

class AudioKeeper {
public:
    AudioKeeper();

    void init(const AudioDirector* audioDirector, const char*, const sead::Vector3f*, const sead::Matrix34f*, const ModelKeeper* modelKeeper);
    void initSeKeeper(const AudioDirector* audioDirector, const char*, const sead::Vector3f*, const sead::Matrix34f*, const ModelKeeper* modelKeeper);
    void initBgmKeeper(const AudioDirector* audioDirector, const char*);
    void initOtherAudio(const AudioDirector* audioDirector);

    void update();
    void validate();
    void invalidate();
    void startClipped();
    void endClipped();
    void appear();
    void kill();
};

class IUseAudioKeeper {
    virtual AudioKeeper* getAudioKeeper() const = 0;
};

} // namespace al