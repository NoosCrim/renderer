#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "OpenGL_utils/vao.hpp"
#include "OpenGL_utils/buffer.hpp"

namespace render
{
    class MeshVAO : public render::VAO
    {
    public:
        enum AttribBinding : GLuint
        {
            POS_BIND,
            COLOR_BIND,
            UV_BIND,
            NORMAL_BIND,
            TANGENT_BIND,
            INSTANCE_TRANSFORM_BIND
        };
        enum AttribIndex : GLuint
        {
            POS_IDX,
            COLOR_IDX,
            UV_IDX,
            NORMAL_IDX,
            TANGENT_IDX,
            INSTANCE_TRANSFORM_COL0_IDX,
            INSTANCE_TRANSFORM_COL1_IDX,
            INSTANCE_TRANSFORM_COL2_IDX,
            INSTANCE_TRANSFORM_COL3_IDX
        };
        MeshVAO() : render::VAO()
        {
            glVertexArrayAttribFormat(name(), POS_IDX, 3, GL_FLOAT, GL_FALSE, 0); // position
            glVertexArrayAttribBinding(name(), POS_IDX, POS_BIND);
            EnableAttrib(POS_BIND);

            glVertexArrayAttribFormat (name(), COLOR_IDX, 4, GL_FLOAT, GL_FALSE, 0); // color
            glVertexArrayAttribBinding(name(), COLOR_IDX, COLOR_BIND);

            glVertexArrayAttribFormat (name(), UV_IDX, 2, GL_FLOAT, GL_FALSE, 0); // uv
            glVertexArrayAttribBinding(name(), UV_IDX, UV_BIND);

            glVertexArrayAttribFormat (name(), NORMAL_IDX, 3, GL_FLOAT, GL_FALSE, 0); // normal
            glVertexArrayAttribBinding(name(), NORMAL_IDX, NORMAL_BIND);

            glVertexArrayAttribFormat (name(), TANGENT_IDX, 3, GL_FLOAT, GL_FALSE, 0); // tangent
            glVertexArrayAttribBinding(name(), TANGENT_IDX, TANGENT_BIND);

            glVertexArrayAttribFormat (name(), INSTANCE_TRANSFORM_COL0_IDX, 4, GL_FLOAT, GL_FALSE, 0); // instance transform col 0
            glVertexArrayAttribFormat (name(), INSTANCE_TRANSFORM_COL1_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*1); // instance transform col 1
            glVertexArrayAttribFormat (name(), INSTANCE_TRANSFORM_COL2_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*2); // instance transform col 2
            glVertexArrayAttribFormat (name(), INSTANCE_TRANSFORM_COL3_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*3); // instance transform col 3
            glVertexArrayAttribBinding(name(), INSTANCE_TRANSFORM_COL0_IDX, INSTANCE_TRANSFORM_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_TRANSFORM_COL1_IDX, INSTANCE_TRANSFORM_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_TRANSFORM_COL2_IDX, INSTANCE_TRANSFORM_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_TRANSFORM_COL3_IDX, INSTANCE_TRANSFORM_BIND);
            glVertexArrayBindingDivisor(name(), INSTANCE_TRANSFORM_BIND, 1);
            EnableAttrib(INSTANCE_TRANSFORM_BIND);
        }
    };

    struct Mesh // separate buffers mesh
    {
    private:
        MeshVAO VAO;
    public:
        GLuint activeVertices = 0;
        TypedSharedBuffer<glm::vec3> vertices;
        TypedSharedBuffer<glm::vec4> colors;
        TypedSharedBuffer<glm::vec3> normals;
        TypedSharedBuffer<glm::vec3> tangents;
        TypedSharedBuffer<glm::vec2> UVs;
        TypedSharedBuffer<GLuint> elements;
        Mesh(GLuint vertCount, glm::vec3 *initialVertsData) :
            activeVertices(vertCount)
        {
            vertices = TypedSharedBuffer<glm::vec3>(activeVertices, initialVertsData);
            VAO.BindVertexBuffer(MeshVAO::POS_BIND, vertices, 0, sizeof(glm::vec3));
        }
        void initColors(glm::vec4 *initialData)
        {
            colors = TypedSharedBuffer<glm::vec4>(vertices.count());
            VAO.EnableAttrib(MeshVAO::COLOR_IDX);
            VAO.BindVertexBuffer(MeshVAO::COLOR_BIND, colors, 0, sizeof(glm::vec4));
        }
        void deinitColors()
        {
            colors = TypedSharedBuffer<glm::vec4>();
            VAO.BindVertexBuffer(MeshVAO::COLOR_IDX);
        }
        void initNormals(glm::vec3 *initialData)
        {
            normals = TypedSharedBuffer<glm::vec3>(vertices.count());
            VAO.EnableAttrib(MeshVAO::NORMAL_IDX);
            VAO.BindVertexBuffer(MeshVAO::NORMAL_BIND, vertices, 0, sizeof(glm::vec3));
        }
        void deinitNormals()
        {
            normals = TypedSharedBuffer<glm::vec3>();
            VAO.DisableAttrib(MeshVAO::NORMAL_IDX);
        }
        void initTangents(glm::vec3 *initialData)
        {
            tangents = TypedSharedBuffer<glm::vec3>(vertices.count());
            VAO.EnableAttrib(MeshVAO::TANGENT_IDX);
            VAO.BindVertexBuffer(MeshVAO::TANGENT_BIND, vertices, 0, sizeof(glm::vec3));
        }
        void deinitTangents()
        {
            tangents = TypedSharedBuffer<glm::vec3>();
            VAO.DisableAttrib(MeshVAO::TANGENT_IDX);
        }
        void initUVs(glm::vec2 *initialData)
        {
            UVs = TypedSharedBuffer<glm::vec2>(vertices.count());
            VAO.EnableAttrib(MeshVAO::UV_IDX);
            VAO.BindVertexBuffer(MeshVAO::UV_BIND, vertices, 0, sizeof(glm::vec2));
        }
        void deinitUVs()
        {
            UVs = TypedSharedBuffer<glm::vec2>();
            VAO.DisableAttrib(MeshVAO::UV_IDX);
        }
        void initElements(GLuint indexCount, GLuint *initialData)
        {
            elements = TypedSharedBuffer<GLuint>(indexCount, initialData);
            VAO.BindVertexBuffer(elements);
        }
        void deinitElements()
        {
            elements = TypedSharedBuffer<GLuint>();
            VAO.BindVertexBuffer(0);
        }
        void Draw(ConstSharedBuffer instanceBuffer)
        {
            VAO.BindVertexBuffer(MeshVAO::INSTANCE_TRANSFORM_BIND, instanceBuffer, 0, sizeof(glm::mat4));
        }
    };
}