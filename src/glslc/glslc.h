#pragma once

#include "nvnTool/nvnTool_GlslcInterface.h"

namespace nn::gfx::detail {
class GlslcDll {
    void* _0;

public:
    GlslcDll();
    ~GlslcDll();
    static GlslcDll* GetInstance(void);

    void Initialize(void);
    void Finalize(void);

    bool IsInitialized(void) const { return _0; }

    bool (*GlslcCompilePreSpecialized)(GLSLCcompileObject*);
    const GLSLCoutput* const* (*GlslcCompileSpecialized)(GLSLCcompileObject*, const GLSLCspecializationBatch*);
    uint8_t (*GlslcInitialize)(GLSLCcompileObject*);
    void (*GlslcFinalize)(GLSLCcompileObject*);
    uint8_t (*GlslcCompile)(GLSLCcompileObject*);
    GLSLCversion (*GlslcGetVersion)();
    void (*GlslcSetAllocator)(GLSLCallocateFunction, GLSLCfreeFunction, GLSLCreallocateFunction, void*);
    GLSLCoptions (*GlslcGetDefaultOptions)();
};

void* GetGlslcCompilePreSpecializedFunction();
void* GetGlslcCompileSpecializedFunction();
void* GetGlslcInitializeFunction();
void* GetGlslcFinalizeFunction();
void* GetGlslcCompileFunction();
void* GetGlslcGetVersionFunction();
void* GetGlslcSetAllocatorFunction();
void* GetGlslcGetDefaultOptionsFunction();
}
