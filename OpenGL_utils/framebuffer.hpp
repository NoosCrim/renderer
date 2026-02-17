#include <cstdio>
#include <GL/glew.h>
#include <vector>
#include <OpenGL_utils/texture.hpp>

namespace render
{
    struct Framebuffer
    {
    protected:
        GLuint _FBO = 0;
        uint32_t _colorAttachmentCount = 0;
        Texture2D *_colorAttachments = nullptr;
        Texture2D _depthAttachment;
    public:
        Framebuffer() = default;
        Framebuffer(uint32_t colorAttachmentCount = 1) :
            _colorAttachmentCount(colorAttachmentCount)
        {
            glCreateFramebuffers(1, &_FBO);
        }
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&& other) noexcept:
            _FBO(other._FBO), _colorAttachmentCount(other._colorAttachmentCount), _colorAttachments(other._colorAttachments), 
            _depthAttachment(other._depthAttachment)
        {
            other._FBO = 0;
            other._colorAttachmentCount = 0;
            other._colorAttachments = nullptr;
            other._depthAttachment = {};
        }
        Framebuffer& operator=(Framebuffer&& other) noexcept
        {
            if (this != &other)
            {
                if (_FBO)
                    glDeleteFramebuffers(1, &_FBO);
                delete[] _colorAttachments;

                _FBO = other._FBO;
                _colorAttachmentCount = other._colorAttachmentCount;
                _colorAttachments = other._colorAttachments;
                _depthAttachment = other._depthAttachment;

                other._FBO = 0;
                other._colorAttachmentCount = 0;
                other._colorAttachments = nullptr;
                other._depthAttachment = {};
            }
            return *this;
        }
        ~Framebuffer()
        {
            if (_FBO)
                glDeleteFramebuffers(1, &_FBO);
            if(_colorAttachments)
                delete[] _colorAttachments;
        }

        inline Texture2D GetDepthAttachment() const
        {
            return _depthAttachment;
        }
        inline Texture2D GetColorAttachment(uint32_t index) const
        {
            if (index >= _colorAttachmentCount)
            {
                fputs("Invalid color attachment index\n", stderr);
                return Texture2D();
            }
            return _colorAttachments[index];
        }
        bool SetColorAttachment(uint32_t index, Texture2D texture)
        {
            if (index >= _colorAttachmentCount)
            {
                fputs("Invalid color attachment index\n", stderr);
                return false;
            }

            _colorAttachments[index] = texture;
            glNamedFramebufferTexture(_FBO, GL_COLOR_ATTACHMENT0 + index, texture.ID(), 0);
            return true;
        }
        bool SetDepthAttachment(Texture2D texture)
        {
            _depthAttachment = texture;
            glNamedFramebufferTexture(_FBO, GL_DEPTH_ATTACHMENT, texture.ID(), 0);
            return true;
        }
        inline GLuint ID() const
        {
            return _FBO;
        }
        inline operator GLuint() const
        {
            return _FBO;
        }
        void Bind() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
        }
        static void Unbind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        static void Bind(const Framebuffer& framebuffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.ID());
        }
    };
}