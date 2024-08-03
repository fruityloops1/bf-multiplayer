#pragma once

#include "al/Execute/ExecuteDirector.h"
#include "al/Functor/FunctorBase.h"

namespace al {

void registerExecutorFunctor(const char* executor, ExecuteDirector* executeDirector, const FunctorBase& functor);
void registerExecutorUser(IUseExecutor* user, ExecuteDirector* executeDirector, const char* executor);

} // namespace al
