#pragma once

namespace al {

class ExecuteTableHolderUpdate;
class ExecuteTableHolderDraw;
class ExecuteRequestKeeper;

class ExecuteDirector {
    int _0;
    int mNumUpdateTables = 0;
    ExecuteTableHolderUpdate* mUpdateTables = nullptr;
    int mNumDrawTables = 0;
    ExecuteTableHolderDraw* mDrawTables = nullptr;
    ExecuteRequestKeeper* mRequestKeeper = nullptr;

public:
};

class IUseExecutor {
public:
    virtual void execute() = 0;
    virtual void draw() { }
};

class ActorExecuteInfo {
};

} // namespace al
