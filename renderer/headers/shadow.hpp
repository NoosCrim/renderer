#pragma once
#include "OpenGL_utils/shader.hpp"
#include "OpenGL_utils/buffer.hpp"
#include "framebuffers.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace render
{
    // Forward declaration — defined in builtin_shader.hpp
    extern const char* shader_location;

    // Must match shadow_shared.glsl
    constexpr GLuint SHADOW_UBO_BINDING = 15;
    constexpr GLuint SHADOW_TEXTURE_UNIT = 15;

    struct ShadowMapPass
    {
    private:
        struct ShadowMapShader : ShaderProgram
        {
            ShadowMapShader() :
                ShaderProgram({
                    Shader::FromFile(GL_VERTEX_SHADER, (std::string{shader_location} + "/shadow_map.vert.glsl").c_str()),
                    Shader::FromFile(GL_FRAGMENT_SHADER, (std::string{shader_location} + "/shadow_map.frag.glsl").c_str())})
            {
            }
        };
        static ShadowMapShader& shadowMapShader() {
            static ShadowMapShader shader; // constructed on first call
            return shader;
        }
        struct ShadowUniformData
        {
            glm::mat4 lightViewProjection;
        };
        TypedSharedBuffer<ShadowUniformData> _uniformBuffer{1};
    public:
        const float bias = 0.0004f;
        ShadowMapBuffer _shadowMap;
        ShadowMapPass() = default;
        ShadowMapPass(const ShadowMapPass&) = delete;
        ShadowMapPass& operator=(const ShadowMapPass&) = delete;
        ShadowMapPass(ShadowMapPass&& other) noexcept :
            _uniformBuffer(std::move(other._uniformBuffer)),
            _shadowMap(std::move(other._shadowMap))
        {}
        ShadowMapPass& operator=(ShadowMapPass&& other) noexcept
        {
            if (this != &other)
            {
                _uniformBuffer = std::move(other._uniformBuffer);
                _shadowMap = std::move(other._shadowMap);
            }
            return *this;
        }

        ShadowMapPass(uint32_t resolution) :
            _shadowMap(resolution)
        {}

        ShadowMapPass(Texture2D depthTexture) :
            _shadowMap(depthTexture)
        {}

        // Call before rendering shadow-casting geometry
        void BeginPass(const glm::vec3& lightDir, const glm::vec3& sceneCenter, float sceneRadius)
        {
            if (!_shadowMap) return;

            glm::vec3 dir = glm::normalize(lightDir);
            glm::vec3 lightPos = sceneCenter + dir * sceneRadius;
            glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 lightProj = glm::ortho(-sceneRadius, sceneRadius,
                                              -sceneRadius, sceneRadius,
                                              0.1f, sceneRadius * 2.0f);

            _uniformBuffer[0].lightViewProjection = lightProj * lightView;

            glBindBufferBase(GL_UNIFORM_BUFFER, SHADOW_UBO_BINDING, _uniformBuffer);

            _shadowMap.Bind();
            glViewport(0, 0, _shadowMap.GetDepthAttachment()->width, _shadowMap.GetDepthAttachment()->height);
            shadowMapShader().Use();
        }
        void Use()
        {
            glBindTextureUnit(SHADOW_TEXTURE_UNIT, _shadowMap.GetDepthAttachment());
            glBindBufferBase(GL_UNIFORM_BUFFER, SHADOW_UBO_BINDING, _uniformBuffer);
        }
    };
}
