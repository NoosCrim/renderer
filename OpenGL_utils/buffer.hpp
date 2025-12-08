#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cstdio>
namespace render
{
    class Buffer
    {
    private:
        GLuint _size = 0;
        GLuint _name = 0;
        void* _data = nullptr;
    public:
        Buffer() = default;
        Buffer(GLsizeiptr size, void* initialData = nullptr) :
            _size(size)
        {
            constexpr GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;
            glGenBuffers(1, &_name);
            if(!_name)
            {
                std::fputs("Failed to generate OpenGL buffer!\n", stderr);
                exit(EXIT_FAILURE);
            }
            glNamedBufferStorage(_name, _size, initialData, flags);
            _data = glMapNamedBufferRange(_name, 0, _size, flags);
            if(!_data)
            {
                std::fputs("Failed to map OpenGL buffer!\n", stderr);
                glDeleteBuffers(1, &_name);
                _name = 0;
                exit(EXIT_FAILURE);
            }
        }
        
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept
            : _size(other._size), _name(other._name), _data(other._data)
        {
            other._size = 0;
            other._name = 0;
            other._data = nullptr;
        }
        Buffer& operator=(Buffer&& other) noexcept
        {
            if (this != &other)
            {
                if(_name)
                {
                    glUnmapNamedBuffer(_name);
                    glDeleteBuffers(1, &_name);
                }
                _size = other._size;
                _name = other._name;
                _data = other._data;
                other._size = 0;
                other._name = 0;
                other._data = nullptr;
            }
            return *this;
        }
        ~Buffer()
        {
            if(_name)
            {
                glUnmapNamedBuffer(_name);
                glDeleteBuffers(1, &_name);
            }
        }
        const void* data() const
        {
            return _data;
        }
        void* data()
        {
            return _data;
        }
        operator GLuint() const
        {
            return _name;
        }
        GLuint size() const
        {
            return _size;
        }
        GLuint name() const
        {
            return _name;
        }
    };
    template<typename T>
    class TypedBuffer : public Buffer
    {
    public:
        TypedBuffer(size_t count, T* initialData = nullptr)
            : Buffer(sizeof(T) * count, initialData)
        {}
        T* data()
        {
            return static_cast<T*>(Buffer::data());
        }
        const T* data() const
        {
            return static_cast<const T*>(Buffer::data());
        }
        size_t count() const
        {
            return Buffer::size() / sizeof(T);
        }
        const T& operator[](size_t index) const
        {
            return data()[index];
        }
        T& operator[](size_t index)
        {
            return data()[index];
        }
    };
}