#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "OpenGL_utils/vao.hpp"
#include "OpenGL_utils/buffer.hpp"

namespace render
{
    struct InstanceData
    {
        glm::mat4 model, inverse_model;
    };
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
            INSTANCE_MODEL_BIND,
            INSTANCE_INVERSE_MODEL_BIND
        };
        enum AttribIndex : GLuint
        {
            POS_IDX,
            COLOR_IDX,
            UV_IDX,
            NORMAL_IDX,
            TANGENT_IDX,
            INSTANCE_MODEL_COL0_IDX,
            INSTANCE_MODEL_COL1_IDX,
            INSTANCE_MODEL_COL2_IDX,
            INSTANCE_MODEL_COL3_IDX,
            INSTANCE_INVERSE_MODEL_COL0_IDX,
            INSTANCE_INVERSE_MODEL_COL1_IDX,
            INSTANCE_INVERSE_MODEL_COL2_IDX,
            INSTANCE_INVERSE_MODEL_COL3_IDX
        };
        
        MeshVAO() : render::VAO()
        {
            glVertexArrayAttribFormat(name(), POS_IDX, 3, GL_FLOAT, GL_FALSE, 0); // position
            glVertexArrayAttribBinding(name(), POS_IDX, POS_BIND);
            EnableAttrib(POS_IDX);

            glVertexArrayAttribFormat (name(), COLOR_IDX, 4, GL_FLOAT, GL_FALSE, 0); // color
            glVertexArrayAttribBinding(name(), COLOR_IDX, COLOR_BIND);

            glVertexArrayAttribFormat (name(), UV_IDX, 2, GL_FLOAT, GL_FALSE, 0); // uv
            glVertexArrayAttribBinding(name(), UV_IDX, UV_BIND);

            glVertexArrayAttribFormat (name(), NORMAL_IDX, 3, GL_FLOAT, GL_FALSE, 0); // normal
            glVertexArrayAttribBinding(name(), NORMAL_IDX, NORMAL_BIND);

            glVertexArrayAttribFormat (name(), TANGENT_IDX, 3, GL_FLOAT, GL_FALSE, 0); // tangent
            glVertexArrayAttribBinding(name(), TANGENT_IDX, TANGENT_BIND);

            glVertexArrayAttribFormat (name(), INSTANCE_MODEL_COL0_IDX, 4, GL_FLOAT, GL_FALSE, 0); // instance transform col 0
            glVertexArrayAttribFormat (name(), INSTANCE_MODEL_COL1_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*1); // instance transform col 1
            glVertexArrayAttribFormat (name(), INSTANCE_MODEL_COL2_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*2); // instance transform col 2
            glVertexArrayAttribFormat (name(), INSTANCE_MODEL_COL3_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*3); // instance transform col 3
            glVertexArrayAttribBinding(name(), INSTANCE_MODEL_COL0_IDX, INSTANCE_MODEL_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_MODEL_COL1_IDX, INSTANCE_MODEL_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_MODEL_COL2_IDX, INSTANCE_MODEL_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_MODEL_COL3_IDX, INSTANCE_MODEL_BIND);
            glVertexArrayBindingDivisor(name(), INSTANCE_MODEL_BIND, 1);
            EnableAttrib(INSTANCE_MODEL_COL0_IDX);
            EnableAttrib(INSTANCE_MODEL_COL1_IDX);
            EnableAttrib(INSTANCE_MODEL_COL2_IDX);
            EnableAttrib(INSTANCE_MODEL_COL3_IDX);

            glVertexArrayAttribFormat (name(), INSTANCE_INVERSE_MODEL_COL0_IDX, 4, GL_FLOAT, GL_FALSE, 0); // instance transform col 0
            glVertexArrayAttribFormat (name(), INSTANCE_INVERSE_MODEL_COL1_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*1); // instance transform col 1
            glVertexArrayAttribFormat (name(), INSTANCE_INVERSE_MODEL_COL2_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*2); // instance transform col 2
            glVertexArrayAttribFormat (name(), INSTANCE_INVERSE_MODEL_COL3_IDX, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*3); // instance transform col 3
            glVertexArrayAttribBinding(name(), INSTANCE_INVERSE_MODEL_COL0_IDX, INSTANCE_INVERSE_MODEL_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_INVERSE_MODEL_COL1_IDX, INSTANCE_INVERSE_MODEL_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_INVERSE_MODEL_COL2_IDX, INSTANCE_INVERSE_MODEL_BIND);
            glVertexArrayAttribBinding(name(), INSTANCE_INVERSE_MODEL_COL3_IDX, INSTANCE_INVERSE_MODEL_BIND);
            glVertexArrayBindingDivisor(name(), INSTANCE_INVERSE_MODEL_BIND, 1);
            EnableAttrib(INSTANCE_INVERSE_MODEL_COL0_IDX);
            EnableAttrib(INSTANCE_INVERSE_MODEL_COL1_IDX);
            EnableAttrib(INSTANCE_INVERSE_MODEL_COL2_IDX);
            EnableAttrib(INSTANCE_INVERSE_MODEL_COL3_IDX);
        }
    };

    struct Mesh // separate buffers mesh
    {
    private:
        MeshVAO VAO;
    public:
        GLuint activeVertices = 0;
        TypedSharedBuffer<FragmentShaderBRDF::UniformData> material;
        TypedSharedBuffer<glm::vec3> vertices;
        TypedSharedBuffer<glm::vec4> colors;
        TypedSharedBuffer<glm::vec3> normals;
        TypedSharedBuffer<glm::vec3> tangents;
        TypedSharedBuffer<glm::vec2> UVs;
        TypedSharedBuffer<GLuint> elements;
        Mesh(GLuint vertCount, const glm::vec3 *initialVertsData) :
            activeVertices(vertCount)
        {
            vertices = TypedSharedBuffer<glm::vec3>(activeVertices, initialVertsData);
            VAO.BindVertexBuffer(MeshVAO::POS_BIND, vertices, 0, sizeof(glm::vec3));
        }
        void initColors(const glm::vec4 *initialData)
        {
            colors = TypedSharedBuffer<glm::vec4>(vertices.count(), initialData);
            VAO.EnableAttrib(MeshVAO::COLOR_IDX);
            VAO.BindVertexBuffer(MeshVAO::COLOR_BIND, colors, 0, sizeof(glm::vec4));
        }
        void deinitColors()
        {
            colors = TypedSharedBuffer<glm::vec4>();
            VAO.DisableAttrib(MeshVAO::COLOR_IDX);
        }
        void initNormals(const glm::vec3 *initialData)
        {
            normals = TypedSharedBuffer<glm::vec3>(vertices.count(), initialData);
            VAO.EnableAttrib(MeshVAO::NORMAL_IDX);
            VAO.BindVertexBuffer(MeshVAO::NORMAL_BIND, vertices, 0, sizeof(glm::vec3));
        }
        void deinitNormals()
        {
            normals = TypedSharedBuffer<glm::vec3>();
            VAO.DisableAttrib(MeshVAO::NORMAL_IDX);
        }
        void initTangents(const glm::vec3 *initialData)
        {
            tangents = TypedSharedBuffer<glm::vec3>(vertices.count(), initialData);
            VAO.EnableAttrib(MeshVAO::TANGENT_IDX);
            VAO.BindVertexBuffer(MeshVAO::TANGENT_BIND, vertices, 0, sizeof(glm::vec3));
        }
        void deinitTangents()
        {
            tangents = TypedSharedBuffer<glm::vec3>();
            VAO.DisableAttrib(MeshVAO::TANGENT_IDX);
        }
        void initUVs(const glm::vec2 *initialData)
        {
            UVs = TypedSharedBuffer<glm::vec2>(vertices.count(), initialData);
            VAO.EnableAttrib(MeshVAO::UV_IDX);
            VAO.BindVertexBuffer(MeshVAO::UV_BIND, vertices, 0, sizeof(glm::vec2));
        }
        void deinitUVs()
        {
            UVs = TypedSharedBuffer<glm::vec2>();
            VAO.DisableAttrib(MeshVAO::UV_IDX);
        }
        void initElements(GLuint indexCount, const GLuint *initialData)
        {
            elements = TypedSharedBuffer<GLuint>(indexCount, initialData);
            VAO.BindElementBuffer(elements);
        }
        void deinitElements()
        {
            elements = TypedSharedBuffer<GLuint>();
            VAO.BindElementBuffer(0);
        }
        void Draw(TypedSharedBuffer<InstanceData> instanceBuffer, GLenum mode = GL_TRIANGLES)
        {
            render::VertexShaderGeneral::uniformBufferData().activeAttribBitfield = VAO.activeAttribBitfield();

            VAO.BindVertexBuffer(MeshVAO::INSTANCE_MODEL_BIND, instanceBuffer, 0, sizeof(glm::mat4)*2);
            VAO.BindVertexBuffer(MeshVAO::INSTANCE_INVERSE_MODEL_BIND, instanceBuffer, sizeof(glm::mat4), sizeof(glm::mat4));
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, material);
            glBindVertexArray(VAO);
            if(elements)
                glDrawElementsInstanced(mode, elements.count(), GL_UNSIGNED_INT, nullptr, instanceBuffer.count());
            else
                glDrawArraysInstanced(mode, 0, vertices.count(), instanceBuffer.count());
        }
    };
}