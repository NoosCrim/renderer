#pragma once
#include "OpenGL_utils/shader.hpp"
#include "OpenGL_utils/buffer.hpp"
#include "OpenGL_utils/texture.hpp"
#include <glm/glm.hpp>

namespace render
{
    enum UniformBufferBindingPoint : GLuint
    {
        CAMERA_BINDING_POINT = 0,
        BRDF_LIGHTING_BINDING_POINT = 1,
        BRDF_MATERIAL_BINDING_POINT = 2
    };
    struct VertexShaderGeneral : Shader
    {
        enum UniformLocation
        {
            ACTIVE_ATRRIB_BIT_LOCATION = 0
        };
        VertexShaderGeneral() : Shader()
        {
            Shader::operator=(Shader::FromFile(GL_VERTEX_SHADER, "./renderer/shader/processed/general.vert.glsl"));
        }
    };
    struct FragmentShaderBRDF : Shader
    {
        enum TextureUnit : GLuint
        {
            ALBEDO_MAP_UNIT,
            ROUGHNESS_MAP_UNIT,
            METALLIC_MAP_UNIT,
            NORMAL_MAP_UNIT,
            AMBIENT_OCCLUSION_MAP_UNIT,
            NEXT_MAP_UNIT
        };
        struct Lighting
        {
        private:
            struct LightUniformData
            {
                glm::vec3 ambientLight;
                float __pad1;
                glm::vec3 lightColor;
                float __pad2;
                glm::vec3 view_lightDirection;
            };
            TypedSharedBuffer<LightUniformData> _lightBuffer{1};
        public:
            LightUniformData &uniformData = _lightBuffer[0];
            void Use() const
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, 1, _lightBuffer);
            }

        };
        FragmentShaderBRDF() : Shader()
        {
            Shader::operator=(Shader::FromFile(GL_FRAGMENT_SHADER, "./renderer/shader/processed/brdf.frag.glsl"));
        }
        struct Material
        {
        private:
            struct MaterialUniformData
            {
                friend class Material;
                glm::vec4 color_mod = { 1.f, 1.f, 1.f, 1.f };
                float roughness_mod = 1.f;
                float metallic_mod = 1.f;
                float ambient_occlusion_mod = 1.f;
                float normal_mod = 1.f; // how much normal map affects the final normal <0,1> range
            protected:
                union
                {
                    struct
                    {
                        mutable uint32_t albedo_active : 1,
                                roughness_active : 1,
                                metallic_active : 1,
                                normal_active : 1,
                                ambient_occlusion_active : 1;
                        uint32_t unused : 27;
                    };
                    mutable uint32_t active_texture_bitfield = 0u;
                };
            };
            render::TypedSharedBuffer<MaterialUniformData> materialBuffer{1};
        public:
            Material()
            {
                materialBuffer[0] = MaterialUniformData{};
            }
            MaterialUniformData &uniformData = materialBuffer[0];
            Texture textures[NEXT_MAP_UNIT];
            void Use() const
            {   
                uniformData.active_texture_bitfield = 0u;
                for(unsigned int i = 0; i < NEXT_MAP_UNIT; i++)
                {
                    uniformData.active_texture_bitfield = uniformData.active_texture_bitfield | ((textures[i]->name != 0) ? (1u << i): 0u);
                    glBindTextureUnit(i, textures[i]);
                }
                glBindBufferBase(GL_UNIFORM_BUFFER, 2, materialBuffer);
            }
        };
    };
    struct ShaderProgramBRDF : ShaderProgram
    {
        ShaderProgramBRDF() : 
            ShaderProgram({
                VertexShaderGeneral(),
                FragmentShaderBRDF()})
        {

        }
    };
}