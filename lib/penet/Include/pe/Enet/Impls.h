#pragma once

#ifdef NNSDK
#include "diag/assert.hpp"
#include "pe/Enet/Enet.h"
#include "pe/Util/Log.h"
#define PENET_ABORT(FMT, ...)      \
    {                              \
        pe::err(FMT, __VA_ARGS__); \
        EXL_ABORT(0x402);          \
    }
#define PENET_WARN(FMT, ...)        \
    {                               \
        pe::warn(FMT, __VA_ARGS__); \
    }
#define PENET_MALLOC(SIZE) pe::enet::getEnetHeap()->alloc(SIZE)
#define PENET_FREE(PTR) pe::enet::getEnetHeap()->free(PTR)
#else
#include "main.h"
#include <cstdio>
#include <cstdlib>
#define PENET_MALLOC(SIZE) buddyMalloc(SIZE)
#define PENET_FREE(PTR) buddyFree(PTR)
#define PENET_ABORT(FMT, ...)              \
    {                                      \
        fprintf(stderr, FMT, __VA_ARGS__); \
        fflush(stderr);                    \
        abort();                           \
    }
#define PENET_WARN(FMT, ...)              \
    {                                     \
        printf(stdout, FMT, __VA_ARGS__); \
    }
#endif