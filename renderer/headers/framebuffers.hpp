#pragma once
#include <GL/glew.h>
#include <cstdio>
#include "OpenGL_utils/framebuffer.hpp"
namespace render
{
    struct ShadowMapBuffer : public Framebuffer
    {
        ShadowMapBuffer() = default;
        ShadowMapBuffer(uint32_t resolution) : Framebuffer(0)
        {
            SetDepthAttachment(Texture2D(1, GL_DEPTH_COMPONENT24, 1, resolution, resolution));
            glNamedFramebufferDrawBuffer(_FBO, GL_NONE);
            glNamedFramebufferReadBuffer(_FBO, GL_NONE);
        }
        ShadowMapBuffer(Texture2D depthTexture) : Framebuffer(0)
        {
            SetDepthAttachment(depthTexture);
            glNamedFramebufferDrawBuffer(_FBO, GL_NONE);
            glNamedFramebufferReadBuffer(_FBO, GL_NONE);
        }
    };

    struct gBuffer : public Framebuffer
    {
    public:
        gBuffer() = default;
        gBuffer(const gBuffer&) = delete;
        gBuffer& operator=(const gBuffer&) = delete;
        gBuffer(gBuffer&& other) noexcept : Framebuffer(std::move(other)) {}
        gBuffer& operator=(gBuffer&& other) noexcept
        {
            Framebuffer::operator=(std::move(other));
            return *this;
        }
        gBuffer(uint32_t width, uint32_t height) : Framebuffer(4)
        {
            SetColorAttachment(ALBEDO, Texture2D(1, GL_RGBA8, 4, width, height));
            SetColorAttachment(NORMAL, Texture2D(1, GL_RGB16F, 3, width, height));
            SetColorAttachment(RMO, Texture2D(1, GL_RGB8, 3, width, height));
            SetColorAttachment(EMISSIVE, Texture2D(1, GL_RGB8, 3, width, height));
            SetDepthAttachment(Texture2D(1, GL_DEPTH_COMPONENT24, 1, width, height));
        }
        enum AttachmentIndices
        {
            ALBEDO,
            NORMAL,
            RMO,
            EMISSIVE
        };
        inline Texture2D albedo()
        {
            return _colorAttachments[ALBEDO];
        }
        inline void albedo(Texture2D texture)
        {
            SetColorAttachment(ALBEDO, texture);
        }
        inline Texture2D normal()
        {
            return _colorAttachments[NORMAL];
        }
        inline void normal(Texture2D texture)
        {
            SetColorAttachment(NORMAL, texture);
        }
        inline Texture2D rmo()
        {
            return _colorAttachments[RMO];
        }
        inline void rmo(Texture2D texture)
        {
            SetColorAttachment(RMO, texture);
        }
        inline Texture2D emissive()
        {
            return _colorAttachments[EMISSIVE];
        }
        inline void emissive(Texture2D texture)
        {
            SetColorAttachment(EMISSIVE, texture);
        }
    };
}