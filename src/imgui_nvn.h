#pragma once

#include "types.h"

#define IMGUINVN_ADDDRAW(FuncBody) nvnImGui::addDrawFunc([]() { \
    FuncBody                                                    \
});

namespace nvnImGui {

extern bool sDisableRender;

typedef void (*ProcDrawFunc)();

void InstallHooks();

bool InitImGui();

void procDraw();

void addDrawFunc(ProcDrawFunc func);
}
