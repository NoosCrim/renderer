#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cstdio>
namespace render
{
    class SharedBuffer;
    class ConstSharedBuffer
    {
    private:
        class __Buffer
        {
            friend class __BufferInterface;
            void* _data = nullptr;
            GLuint _size = 0;
            GLuint _name = 0;
            mutable GLuint _refCount = 0;
        public:
            __Buffer(const __Buffer&) = delete;
            __Buffer& operator=(const __Buffer&) = delete;

            __Buffer(GLsizeiptr size, void* initialData = nullptr);
            __Buffer(__Buffer&& other) noexcept;
            __Buffer& operator=(__Buffer&& other) noexcept;
            ~__Buffer();
            inline void Aquire() const
            {
                ++_refCount;
            }
            inline void Release() const
            {
                if(--_refCount == 0) delete this;
            }
            inline const void* data() const
            {
                return _data;
            }
            inline void* data()
            {
                return _data;
            }
            inline operator GLuint() const
            {
                return _name;
            }
            inline operator bool() const
            {
                return _name;
            }
            inline GLuint size() const
            {
                return _size;
            }
            inline GLuint name() const
            {
                return _name;
            }
        };
    protected:
        __Buffer *buffer = nullptr;
    public:
        ConstSharedBuffer() = default;
        ConstSharedBuffer(const ConstSharedBuffer &other);
        ConstSharedBuffer& operator=(const ConstSharedBuffer &other);
        ConstSharedBuffer(GLsizeiptr size, void* initialData = nullptr);
        ConstSharedBuffer(ConstSharedBuffer&& other) noexcept;
        ConstSharedBuffer& operator=(ConstSharedBuffer&& other) noexcept;
        inline GLuint size() const
        {
            return buffer ? buffer->size() : 0;
        }
        inline const void* data() const
        {
            return buffer ? buffer->data() : nullptr;
        }
        inline GLuint name() const
        {
            return buffer ? buffer->name() : 0;
        }
        inline operator GLuint() const
        {
            return name();
        }
        inline operator bool() const
        {
            return name();
        }
        ~ConstSharedBuffer()
        {
            if(buffer) buffer->Release();
        }
    };
    
    template<typename T>
    class TypedConstSharedBuffer : public ConstSharedBuffer
    {
        TypedConstSharedBuffer() = default;
        TypedConstSharedBuffer(const TypedConstSharedBuffer &other) : ConstSharedBuffer(other) {}
        TypedConstSharedBuffer& operator=(const TypedConstSharedBuffer &other)
        {
            ConstSharedBuffer::operator=(other);
            return *this;
        }
        TypedConstSharedBuffer(GLsizeiptr count, T* initialData = nullptr) : ConstSharedBuffer(count * sizeof(T), initialData) {}
        TypedConstSharedBuffer(ConstSharedBuffer&& other) noexcept : ConstSharedBuffer(other) {}
        TypedConstSharedBuffer& operator=(TypedConstSharedBuffer&& other) noexcept
        {
            ConstSharedBuffer::operator=(&&other);
            return *this;
        }

        inline GLuint count() const
        {
            return buffer ? buffer->size()/sizeof(T) : 0;
        }
        inline const T* data() const
        {
            return buffer ? buffer->data() : nullptr;
        }
    };

    class SharedBuffer : public ConstSharedBuffer
    {
    public:
        SharedBuffer() = default;
        SharedBuffer(const SharedBuffer &other) : ConstSharedBuffer(other) {}
        SharedBuffer& operator=(const SharedBuffer &other)
        {
            ConstSharedBuffer::operator=(other);
            return *this;
        }
        SharedBuffer(GLsizeiptr size, void* initialData = nullptr) : ConstSharedBuffer(size, initialData) {}
        SharedBuffer(SharedBuffer&& other) noexcept : ConstSharedBuffer(std::move(other)) {}
        SharedBuffer& operator=(SharedBuffer&& other) noexcept
        {
            ConstSharedBuffer::operator=(std::move(other));
            return *this;
        }
        inline void* data()
        {
            return buffer ? buffer->data() : nullptr;
        }
    };

    template<typename T>
    class TypedSharedBuffer : public SharedBuffer
    {
    public:
        TypedSharedBuffer() = default;
        TypedSharedBuffer(TypedSharedBuffer &other) : SharedBuffer(other) {}
        TypedSharedBuffer& operator=(TypedSharedBuffer &other)
        {
            ConstSharedBuffer::operator=(other);
            return *this;
        }
        TypedSharedBuffer(GLsizeiptr count, T* initialData = nullptr) : SharedBuffer(count * sizeof(T), initialData) {}
        TypedSharedBuffer(TypedSharedBuffer&& other) noexcept : SharedBuffer(other) {}
        TypedSharedBuffer& operator=(TypedSharedBuffer&& other) noexcept
        {
            SharedBuffer::operator=(&&other);
            return *this;
        }

        inline GLuint count() const
        {
            return buffer ? buffer->size()/sizeof(T) : 0;
        }
        inline T* data()
        {
            return buffer ? buffer->data() : nullptr;
        }
    };
}