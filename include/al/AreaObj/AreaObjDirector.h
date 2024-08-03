#pragma once

namespace al {

class AreaObjDirector {
};

class IUseAreaObj {
    virtual AreaObjDirector* getAreaObjDirector() const = 0;
};

} // namespace al