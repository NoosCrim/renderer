#include "buffer.hpp"
namespace render
{
    ConstSharedBuffer::__Buffer::__Buffer(GLsizeiptr size, const void* initialData) :
        _size(size)
    {
        constexpr GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;
        glCreateBuffers(1, &_name);
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
    ConstSharedBuffer::__Buffer::__Buffer(__Buffer&& other) noexcept
        : _data(other._data), _size(other._size), _name(other._name)
    {
        other._size = 0;
        other._name = 0;
        other._data = nullptr;
    }
    ConstSharedBuffer::__Buffer& ConstSharedBuffer::__Buffer::operator=(ConstSharedBuffer::__Buffer&& other) noexcept
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
    ConstSharedBuffer::__Buffer::~__Buffer()
    {
        if(_name)
        {
            glUnmapNamedBuffer(_name);
            glDeleteBuffers(1, &_name);
        }
    }

    ConstSharedBuffer::ConstSharedBuffer(const ConstSharedBuffer &other)
    {
        if(other.buffer)
        {
            buffer = other.buffer;
            buffer->Acquire();
        }
    }
    ConstSharedBuffer& ConstSharedBuffer::operator=(const ConstSharedBuffer &other)
    {
        if(this != &other)
        {
            if(buffer)
            {
                buffer->Release();
            }
            buffer = other.buffer;
            if(buffer)
            {
                buffer->Acquire();
            }
        }
        return *this;
    }
    ConstSharedBuffer::ConstSharedBuffer(GLsizeiptr size, const void* initialData)
    {
        buffer = new __Buffer(size, initialData);
        buffer->Acquire();
    }
    ConstSharedBuffer::ConstSharedBuffer(ConstSharedBuffer&& other) noexcept
    {
        buffer = other.buffer;
        other.buffer = nullptr;
    }
    ConstSharedBuffer& ConstSharedBuffer::operator=(ConstSharedBuffer&& other) noexcept
    {
        if(this != &other)
        {
            if(buffer)
            {
                buffer->Release();
            }
            buffer = other.buffer;
            other.buffer = nullptr;
        }
        return *this;
    }
}