#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "transform.hpp"

namespace render
{
    struct Camera
    {
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
                float aspectRatio;
            };
        };
    private:
        glm::mat4 _projection;
        uint16_t _lastTransformVersion = (uint16_t)-1;
        bool _projectionDirty = true;
        enum CameraType : uint8_t
        {
            Perspective,
            Orthographic,
            Custom
        } cameraType = Perspective;
    public:
        Camera() : fov(60.f), aspectRatio(16.f/9.f) // default perspective parameters
        {}
        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        Camera& operator=(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;

        void perspective(float fovDegrees, float aspect)
        {
            if (fovDegrees == fov && aspect == aspectRatio && cameraType == Perspective)
                return;
            fov = fovDegrees;
            aspectRatio = aspect;
            cameraType = Perspective;
            _projectionDirty = true;
        }
        void perspective(float *fovDegrees, float *aspect) const
        {
            *fovDegrees = fov;
            *aspect = aspectRatio;
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
                    _projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
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