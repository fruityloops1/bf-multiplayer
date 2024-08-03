#pragma once

#include "al/Scene/ISceneObj.h"

namespace al {

class SceneObjHolder {
    using CreateFunc = ISceneObj* (*)(int id);

    CreateFunc mCreateFunc;
    ISceneObj** mObjs;
    int mSize;

public:
    SceneObjHolder(CreateFunc func, int size);

    void setObj(ISceneObj* obj, int id);
    ISceneObj* getObj(int id);
    ISceneObj* create(int id);

    void initAfterPlacementSceneObj(const ActorInitInfo& info);
};

class IUseSceneObjHolder {
public:
    virtual SceneObjHolder* getSceneObjHolder() const = 0;
};

void createSceneObj(IUseSceneObjHolder*, int index);
void setSceneObj(IUseSceneObjHolder*, ISceneObj*, int index);
ISceneObj* getSceneObj(IUseSceneObjHolder*, int index);

} // namespace al