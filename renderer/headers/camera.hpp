#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "transform.hpp"
#include "builtin_shader.hpp"

namespace render
{
    struct Camera
    {
    private:
        struct CameraUniformData
        {
            glm::mat4 inverse_view;
            glm::mat4 view;
            glm::mat4 projection;
            glm::uvec2 resolution;
        };
        TypedSharedBuffer<CameraUniformData> _cameraBuffer{1};
    public:
        Transform transform;
        float nearPlane = 0.1f;
        float farPlane = 1000.f;
        union 
        {
            struct
            {
                float left;
                float right;
                float bottom;
                float top;
            };
            struct
            {
                float fov;
            };
        };
        glm::uvec2 &resolution; // ref to resolution in CameraUniformData
    private:
        glm::mat4 &_projection;
        bool _projectionDirty = true;
        enum CameraType : uint8_t
        {
            Perspective,
            Orthographic,
            Custom
        } cameraType = Perspective;
    public:
        Camera() :
            transform(_cameraBuffer, &_cameraBuffer[0].inverse_view, &_cameraBuffer[0].view),
            fov(60.f), 
            resolution(_cameraBuffer[0].resolution),
            _projection(_cameraBuffer[0].projection)
        {}
        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        Camera& operator=(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;
        void Use() const
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING_POINT, _cameraBuffer);
        }
        void perspective(float fovDegrees, uint32_t w, uint32_t h)
        {
            if (fovDegrees == fov && w == resolution.x && h == resolution.y && cameraType == Perspective)
                return;
            fov = fovDegrees;
            resolution.x = w;
            resolution.y = h;
            cameraType = Perspective;
            _projectionDirty = true;
        }
        void perspective(float *fovDegrees, uint32_t *w, uint32_t *h) const
        {
            *fovDegrees = fov;
            *w = resolution.x;
            *h = resolution.y;
        }

        void ortho(float l, float r, float b, float t)
        {
            if (l == left && r == right && 
                b == bottom && t == top && 
                cameraType == Orthographic)
                return;
            left = l;
            right = r;
            bottom = b;
            top = t;
            cameraType = Orthographic;
            _projectionDirty = true;
        }
        void ortho(float *l, float *r, float *b, float *t) const
        {
            *l = left;
            *r = right;
            *b = bottom;
            *t = top;
        }

        void customProjection(const glm::mat4& proj)
        {   
            if(proj == _projection && cameraType == Custom)
                return;
            _projection = proj;
            cameraType = Custom;
            _projectionDirty = false;
        }

        void setFarNear(float nearP, float farP)
        {
            if (nearP == nearPlane && farP == farPlane)
                return;
            nearPlane = nearP;
            farPlane = farP;
            _projectionDirty = true;
        }
        void getFarNear(float *nearP, float *farP) const
        {
            *nearP = nearPlane;
            *farP = farPlane;
        }

        const glm::mat4& projection()
        {
            if (_projectionDirty)
            {
                if(cameraType == Perspective)
                {
                    _projection = glm::perspective(glm::radians(fov), (float)resolution.x / (float)resolution.y, nearPlane, farPlane);
                }
                else if(cameraType == Orthographic)
                {
                    _projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
                }
                // else Custom: do nothing, user sets projection manually

                _projectionDirty = false;
            }
            return _projection;
        }

        const glm::mat4& view()
        {
            return transform.inverse();
        }
        
        const glm::mat4& inverse_view()
        {
            return transform.matrix();
        }
    };
}