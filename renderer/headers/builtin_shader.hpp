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
    private:
        TypedSharedBuffer<VertexShaderGeneral::UniformData> _vertexUniformBuffer;
    public:
        TypedSharedBuffer<VertexShaderGeneral::UniformData> vertexUniformBuffer()
        {
            return _vertexUniformBuffer;
        }
        void vertexUniformBuffer(TypedSharedBuffer<VertexShaderGeneral::UniformData> vertexUniformBuff)
        {
            _vertexUniformBuffer = vertexUniformBuff;
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, _vertexUniformBuffer);
        }
        ShaderProgramBRDF(TypedSharedBuffer<VertexShaderGeneral::UniformData> vertexUniformBuff) : 
            ShaderProgram({
                VertexShaderGeneral(),
                FragmentShaderBRDF()}),
            _vertexUniformBuffer(vertexUniformBuff)
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, _vertexUniformBuffer);
        }
    };
}