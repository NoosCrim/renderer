#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace render
{
    struct Transform
    {
    private:
        mutable glm::mat4 _matrix;
        mutable glm::mat4 _inverse;
        glm::vec3 _position;
        glm::quat _orientation;
        glm::vec3 _scale;
        uint16_t _version = 0u;
        mutable bool matrixDirty = true;
        mutable bool inverseDirty = true;
    public:
        Transform()
            : _position(0.f), _orientation(1.f, 0.f, 0.f, 0.f), _scale(1.f) {}
        
        const uint16_t version() const
        {
            return _version;
        }

        const glm::mat4& matrix() const
        {
            if (matrixDirty)
            {
                glm::mat4 translation = glm::translate(glm::mat4(1.f), _position);
                glm::mat4 rotation = glm::toMat4(_orientation);
                glm::mat4 scaling = glm::scale(glm::mat4(1.f), _scale);
                _matrix = translation * rotation * scaling;
                matrixDirty = false;
            }
            return _matrix;
        }
        const glm::mat4& inverse() const
        {
            if (inverseDirty)
            {
                glm::mat4 translation = glm::translate(glm::mat4(1.f), -_position);
                glm::mat4 rotation = glm::toMat4(glm::inverse(_orientation));
                glm::mat4 scaling = glm::scale(glm::mat4(1.f), 1.f/_scale);
                _inverse = scaling * rotation * translation;
                inverseDirty = false;
            }
            return _inverse;
        }
        const glm::vec3& position() const
        {
            return _position;
        }
        void position(const glm::vec3& pos)
        {
            if(pos == _position) return;
            _position = pos;
            matrixDirty = true;
            inverseDirty = true;
            ++_version;
        }

        const glm::quat& orientation() const
        {
            return _orientation;
        }
        void orientation(const glm::quat& orient)
        {
            if(orient == _orientation) return;
            _orientation = orient;
            matrixDirty = true;
            inverseDirty = true;
            ++_version;
        }

        const glm::vec3 rotation() const // as 
        {
            glm::vec3 eulerAngles;
            glm::extractEulerAngleYXZ(glm::toMat4(_orientation), 
                eulerAngles.y, eulerAngles.x, eulerAngles.z);
            return eulerAngles;
        }
        
        const glm::vec3 rotationDegrees() const
        {
            return glm::degrees(rotation());
        }

        const glm::vec3& scale() const
        {
            return _scale;
        }
        void scale(const glm::vec3& scl)
        {
            if(scl == _scale) return;
            _scale = scl;
            matrixDirty = true;
            inverseDirty = true;
            ++_version;
        }
    };
}