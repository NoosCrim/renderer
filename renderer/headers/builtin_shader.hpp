#pragma once
#include "OpenGL_utils/shader.hpp"
#include "OpenGL_utils/buffer.hpp"
#include <glm/glm.hpp>

namespace render
{
    struct VertexShaderGeneral : Shader
    {
        struct UniformData
        {
            glm::mat4 inverse_view;
            glm::mat4 view;
            glm::mat4 projection;
            uint activeAttribBitfield;
            // glm::vec2 resolution;
            // glm::vec2 time_deltaTime;
        };
        VertexShaderGeneral() : Shader()
        {
            Shader::operator=(Shader::FromFile(GL_VERTEX_SHADER, "./renderer/shader/processed/general.vert.glsl"));
        }
    private:
        static bool _uniformBufferInitialized;
        static TypedSharedBuffer<VertexShaderGeneral::UniformData> _uniformBuffer;
        inline static void _InitializeBufferData()
        {
            _uniformBufferInitialized = true;
            _uniformBuffer = TypedSharedBuffer<VertexShaderGeneral::UniformData>{1};
            Camera cam;
            _uniformBuffer[0].view = cam.view();
            _uniformBuffer[0].inverse_view = cam.inverse_view();
            _uniformBuffer[0].projection = cam.projection();
        }
    public:
        inline static UniformData &uniformBufferData()
        {
            if(!_uniformBufferInitialized) 
            {
                _InitializeBufferData();
            }
            return _uniformBuffer[0];
        }

        inline static 
        TypedSharedBuffer<UniformData>&& swapUniformBuffer(TypedSharedBuffer<VertexShaderGeneral::UniformData> buff)
        {
            _uniformBufferInitialized = true;

            static auto temp = std::move(_uniformBuffer);
            _uniformBuffer = buff;
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uniformBuffer);

            return std::move(temp);
        }
    };
    struct FragmentShaderBRDF : Shader
    {
        struct UniformData
        {
            glm::vec3 ambientLight;
            uint activeTextureBitfield;
            glm::vec3 lightColor;
            float __pad2;
            glm::vec3 view_lightDirection;
        };
        FragmentShaderBRDF() : Shader()
        {
            Shader::operator=(Shader::FromFile(GL_FRAGMENT_SHADER, "./renderer/shader/processed/brdf.frag.glsl"));
        }
        static void BindUniformBuffer(SharedBuffer buffer)
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, buffer);
        }
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