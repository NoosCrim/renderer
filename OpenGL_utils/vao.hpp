#include <GL/glew.h>
namespace render
{
    class VAO
    {
    private:
        GLuint _name;
    protected:
        GLuint _activeAttribBitfield = 0u;
    public:
        VAO()
        {
            glCreateVertexArrays(1, &_name);
        }
        ~VAO()
        {
            glDeleteVertexArrays(1, &_name);
        }
        VAO(const VAO&) = delete;
        VAO& operator=(const VAO&) = delete;
        VAO(VAO&& other) noexcept : _name(other._name), _activeAttribBitfield(other._activeAttribBitfield)
        {
            other._name = 0;
            other._activeAttribBitfield = 0u;
        }
        VAO& operator=(VAO&& other) noexcept
        {
            if (this != &other)
            {
                if(_name) glDeleteVertexArrays(1, &_name);
                _name = other._name;
                _activeAttribBitfield = other._activeAttribBitfield;
                other._name = 0;
                other._activeAttribBitfield = 0u;
            }
            return *this;
        }
        inline GLuint name() const
        {
            return _name;
        }
        inline operator GLuint() const
        {
            return _name;
        }
        inline operator bool() const
        {
            return _name;
        }
        GLuint activeAttribBitfield()
        {
            return _activeAttribBitfield;
        }
        inline bool IsAttribActive(GLuint attribIndex) const
        {
            return (_activeAttribBitfield & (1u << attribIndex)) != 0u;
        }
        inline void EnableAttrib(GLuint attribIndex)
        {
            _activeAttribBitfield |= (1u << attribIndex);
            glEnableVertexArrayAttrib(_name, attribIndex);
        }
        inline void DisableAttrib(GLuint attribIndex)
        {
            _activeAttribBitfield &= ~(1u << attribIndex);
            glDisableVertexArrayAttrib(_name, attribIndex);
        }
        inline void BindVertexBuffer(GLuint bindingIndex, GLuint bufferName, GLintptr offset, GLsizei stride)
        {
            glVertexArrayVertexBuffer(_name, bindingIndex, bufferName, offset, stride);
        }
        inline void BindElementBuffer(GLuint bufferName)
        {
            glVertexArrayElementBuffer(_name, bufferName);
        }
        inline void Draw(GLuint first, GLuint count, GLenum mode = GL_TRIANGLES)
        {
            glDrawArrays(mode, first, count);
        }
        inline void DrawInstanced(GLuint first, GLuint count, GLuint instanceCount, GLenum mode = GL_TRIANGLES)
        {
            glDrawArraysInstanced(mode, first, count, instanceCount);
        }

    };
} // namespace name
