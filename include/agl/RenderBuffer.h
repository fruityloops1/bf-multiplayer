/**
 * @file RenderBuffer.h
 * @brief Defines classes that implement a render buffer.
 */

#pragma once

#include <sead/gfx/seadColor.h>
#include <sead/gfx/seadFrameBuffer.h>
#include <sead/math/seadBoundBox.h>
#include <sead/math/seadVector.h>

#include "agl/DrawContext.h"
#include "agl/RenderTargetColor.h"
#include "agl/RenderTargetDepth.h"

namespace agl {

class RenderBuffer // : public sead::FrameBuffer
{
public:
    RenderBuffer();
    RenderBuffer(const sead::Vector2<f32>&, sead::BoundBox2<f32> const&);
    RenderBuffer(const sead::Vector2<f32>&, f32, f32, f32, f32);
    virtual ~RenderBuffer();

    virtual void copyToDisplayBuffer(sead::DrawContext*, sead::DisplayBuffer const*);
    virtual void clear(sead::DrawContext*, u32, sead::Color4f const&, f32, u32);
    virtual void bindImpl_(sead::DrawContext*) const;

    void initialize_();
    void setRenderTargetColorNullAll();
    void adjustPhysicalAreaAndVirtualSizeFromColorTarget(u32);
    void invalidateGPUCache(agl::DrawContext*) const;
    void bind_(agl::DrawContext*, u16) const;

    u32 _8;
    u32 _C;
    u64 _10;
    u32 _18;
    u32 _1C;
    agl::RenderTargetColor* _20;
    agl::RenderTargetColor* _28;
    agl::RenderTargetColor* _30;
    agl::RenderTargetColor* _38;
    agl::RenderTargetColor* _40;
    agl::RenderTargetColor* _48;
    agl::RenderTargetColor* _50;
    agl::RenderTargetColor* _58;
    agl::RenderTargetDepth* _60;
};

};
