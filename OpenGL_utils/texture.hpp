#pragma once
#include <type_traits>
#include "external/stb/stb_image.h"
#include "external/stb/stb_image_write.h"
#include "GL/glew.h"
#include <string>

namespace render
{
    enum class TexCompType : GLenum
    {
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
        FLOAT = GL_FLOAT
    };
    enum class TexFormat : GLenum
    {
        UNKNOWN = 0,
        R = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA
    };
    static constexpr TexFormat compNumToFormat[] = {
        TexFormat::UNKNOWN,
        TexFormat::R,
        TexFormat::RG,
        TexFormat::RGB,
        TexFormat::RGBA
    };
    constexpr uint8_t TexCompTypeSize(TexCompType type)
    {
        switch(type)
        {
            case TexCompType::UNSIGNED_BYTE:
                return sizeof(GLubyte);
            case TexCompType::UNSIGNED_SHORT:
                return sizeof(GLushort);
            case TexCompType::FLOAT:
                return sizeof(GLfloat);
        }
        return 0;
    }
    
    class Image
    {
        struct ImageData
        {
            void* const pixels;
            const uint32_t width, height, depth, comp_num;
            const TexCompType comp_type;
            void Print()
            {
                std::printf("width: %u; height: %u; depth: %u; comp_num: %u; comp_type: %s\n",
                    width, height, depth, comp_num,
                    comp_type == TexCompType::UNSIGNED_BYTE ? "UNSIGNED_BYTE" : 
                    comp_type == TexCompType::UNSIGNED_SHORT ? "UNSIGNED_SHORT" : 
                    comp_type == TexCompType::FLOAT ? "FLOAT" : "UNKNOWN");
            }
        };
        class ImageDataInterface : public ImageData
        {
        private:
            uint32_t ref_count;
            static void* AllocData(TexCompType comp_type, uint32_t w, uint32_t h, uint32_t d, uint32_t comp_n)
            {
                switch(comp_type)
                {
                    case TexCompType::UNSIGNED_BYTE:
                        return new GLubyte[comp_n * w * h * d];
                    case TexCompType::UNSIGNED_SHORT:
                        return new GLushort[comp_n * w * h * d];
                    case TexCompType::FLOAT:
                        return new GLfloat[comp_n * w * h * d];
                }
                return nullptr;
            }
            void DestroyData()
            {
                switch(comp_type)
                {
                    case TexCompType::UNSIGNED_BYTE:
                        delete[] (GLubyte*) pixels;
                        break;
                    case TexCompType::UNSIGNED_SHORT:
                        delete[] (GLushort*) pixels;
                        break;
                    case TexCompType::FLOAT:
                        delete[] (GLfloat*) pixels;
                        break;
                }
            }
        public:
            ImageDataInterface(TexCompType comp_type, uint32_t w, uint32_t h, uint32_t d, uint32_t comp_n) : 
                ImageData({AllocData(comp_type, w, h, d, comp_n), w, h, d, comp_n, comp_type})
            {

            }
            inline void Acquire()
            {
                ++ref_count;
            }
            inline void Release()
            {
                if(!--ref_count) delete this;
            }
        };
        ImageDataInterface *_data = nullptr;;
    public:
        Image() = default;
        Image(TexCompType comp_type, uint32_t w, uint32_t h, uint32_t d, uint32_t comp_n, const void* initial_data = nullptr) :
            _data(new ImageDataInterface(comp_type, w, h, d, comp_n))
        {
            _data->Acquire();
            if(initial_data)
            {
                std::copy((uint8_t*)initial_data, 
                (uint8_t*)initial_data + w * h * comp_n * TexCompTypeSize(comp_type), 
                (uint8_t*)_data->pixels);
            }
        }
        Image(Image& other)
        {
            _data = other._data;
            _data->Acquire();
        }
        Image(Image&& other)
        {
            _data = other._data;
            other._data = nullptr;
        }
        Image& operator=(Image& other)
        {
            if(_data)
                _data->Release();
            _data = other._data;
            _data->Acquire();
            return *this;
        }
        Image& operator=(Image&& other)
        {
            if(_data)
                _data->Release();
            _data = other._data;
            other._data = nullptr;
            return *this;
        }
        ImageData* operator->()
        {
            return _data;
        }
        const ImageData* operator->() const
        {
            return _data;
        }
        void Store(uint64_t bytes, const void* data)
        {
            uint64_t storage = _data->width * _data->height * _data->comp_num * TexCompTypeSize(_data->comp_type);
            uint64_t copy_bytes = bytes > storage ? storage : bytes;
            std::copy((uint8_t*)data, (uint8_t*)data + copy_bytes, (uint8_t*)_data->pixels);
        }
        static Image FromFile(TexCompType comp_type, char const *filename, int desired_channels = 0)
        {
            int w, h, comp_n;
            void *stb_image;
            switch(comp_type)
            {
                case TexCompType::UNSIGNED_BYTE:
                    stb_image = (uint8_t*)stbi_load(filename, &w, &h, &comp_n, desired_channels);
                    break;
                case TexCompType::UNSIGNED_SHORT:
                    stb_image = (uint8_t*)stbi_load_16(filename, &w, &h, &comp_n, desired_channels);
                    break;
                case TexCompType::FLOAT:
                    stb_image = (uint8_t*)stbi_loadf(filename, &w, &h, &comp_n, desired_channels);
                    break;
            }
            Image image{comp_type, (uint32_t)w, (uint32_t)h, 1u, (uint32_t)comp_n, stb_image};
            stbi_image_free(stb_image);
            return image;
        }
    };
    class Texture
    {
    // member structs
    protected:
        struct TextureMetadata
        {
        public:
            // w, h, d, n, levels - width, height, depth, number of components, amount of mipmap levels
            const GLuint name = 0;
            const GLenum target, internal_format;
            const GLsizei width, height, depth, comp_num, levels;
        };
        struct TextureMetadataInterface : public TextureMetadata
        {
        private:
            mutable uint32_t _ref_count = 0;
            static GLuint _createTexture(GLenum target)
            {
                GLuint name;
                glCreateTextures(target, 1, &name);
                return name;
            }
        public:
            inline void Acquire()
            {
                ++_ref_count;
            }
            inline void Release()
            {
                if(!--_ref_count)
                {
                    delete this;
                }
            }
            inline TextureMetadataInterface(GLenum gl_target, GLsizei lvls, GLenum gl_in_format,
                               GLsizei comp_n, GLsizei w, GLsizei h = 1, GLsizei d = 1) : 
                TextureMetadata({_createTexture(gl_target), 
                            gl_target, gl_in_format,
                            w, h, d, comp_n, lvls})

            {
                
            }

            ~TextureMetadataInterface()
            {
                glDeleteTextures(1, &name);
            }
        };

    // member fields and functions
    private:
        TextureMetadataInterface* _data = nullptr;
    protected:
        Texture(TextureMetadataInterface* data) :
            _data(data)
        {
            _data->Acquire();
        }
        Texture(GLenum gl_target, GLsizei lvls, GLenum gl_in_format,
                GLsizei comp_n, GLsizei w, GLsizei h = 1, GLsizei d = 1) :
            _data(new TextureMetadataInterface{ gl_target, lvls, gl_in_format, comp_n, w, h, d})
        {
            _data->Acquire();
        }
    public:
        Texture() = default;

        Texture(const Texture &other)
        {
            _data = other._data;
            if(_data) 
                _data->Acquire();
        }
        Texture(Texture &&other)
        {
            _data = other._data;
            other._data = nullptr;
        }
        Texture& operator=(const Texture& other)
        {
            if(_data == other._data) return *this;
            if(_data)
                _data->Release();
            _data = other._data;
            if(_data)
                _data->Acquire();
            return *this;
        }
        Texture& operator=(Texture&& other)
        {
            if(_data)
                _data->Release();
            _data = other._data;
            other._data = nullptr;
            return *this;
        }
        const TextureMetadata* operator->() const
        {
            static TextureMetadata uninit = {0,0,0,0,0,0,0,0};
            return _data ? _data : &uninit;
        }
        inline operator GLuint() const
        {
            return _data ? _data->name : 0;
        }
        inline operator bool() const
        {
            return operator GLuint();
        }
        const TextureMetadata* data() const
        {
            return _data;
        }
    };
    class Texture2D : public Texture
    {
    public:
        // TODO - add constructor from Image
        Texture2D() = default;
        Texture2D(Texture2D& other) : Texture(other) {}
        Texture2D(Texture2D&& other) : Texture(std::move(other)) {}
        Texture2D(GLsizei lvls, GLenum gl_in_format,
                  GLsizei comp_n, GLsizei w, GLsizei h):
            Texture(GL_TEXTURE_2D, lvls, gl_in_format, comp_n, w, h)
        {
            glTextureStorage2D(data()->name, data()->levels, data()->internal_format, data()->width, data()->height);
        }
        Texture2D(Image image, GLsizei lvls, GLenum gl_in_format):
            Texture(GL_TEXTURE_2D, lvls, gl_in_format, image->comp_num, image->width, image->height)
        {
            glTextureStorage2D(data()->name, data()->levels, data()->internal_format, data()->width, data()->height);
            glTextureSubImage2D(
                data()->name, 0, 0, 0, data()->width, data()->width, 
                (GLenum)compNumToFormat[data()->comp_num], (GLenum)image->comp_type, image->pixels);
        }
        inline void Load(TexFormat format, TexCompType type, void* pixels, GLint level, GLint x, GLint y, GLsizei w, GLsizei h)
        {
            glTextureSubImage2D(data()->name, level, x, y, w, h, (GLenum)format, (GLenum)type, pixels);
        }
        inline void Load(TexFormat format, TexCompType type, void* pixels, GLint level)
        {
            glTextureSubImage2D(data()->name, level, 0, 0, data()->width, data()->width, (GLenum)format, (GLenum)type, pixels);
        }
        inline void Load(const Image image, GLint level, GLint x, GLint y)
        {
            glTextureSubImage2D(
                data()->name, level, x, y, data()->width, data()->width, 
                (GLenum)(GLenum)compNumToFormat[data()->comp_num], (GLenum)image->comp_type, image->pixels);
        }
        inline void Load(const Image image, GLint level)
        {
            glTextureSubImage2D(
                data()->name, level, 0, 0, data()->width, data()->width, 
                (GLenum)compNumToFormat[data()->comp_num], (GLenum)image->comp_type, image->pixels);
        }
        inline void Fetch(TexFormat format, TexCompType type, GLint level, GLsizei bufSize, void* pixels)
        {
            glGetTextureImage(data()->name, level, (GLenum)format, (GLenum)type, bufSize, pixels);
        }
    };
}