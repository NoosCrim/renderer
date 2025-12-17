#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer/renderer.hpp"

#include "demo_utils.hpp"

static void glfw_error_callback(int error, const char* description)
{
    (void)error;
	std::fprintf(stderr, "GLFW error: %s\n", description);
}
static void GLAPIENTRY gl_error_callback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
{
    (void)source;
    (void)id;
    (void)length;
    (void)userParam;
	if(type == GL_DEBUG_TYPE_ERROR)
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
}
int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit())
    {
        const char *errorString;
        glfwGetError(&errorString);
        printf("GLFW initialization failed: %s\n", errorString);
        return 1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "renderer demo", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    GLenum glewInitCode = glewInit();
    if(glewInitCode != GLEW_OK)
    {
        printf("GLEW initialization failed: %s\n", glewGetErrorString(glewInitCode));
        return 1;
    }
    glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_error_callback, nullptr);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    // renderer setup
    render::ShaderProgramBRDF shaderBRDF{render::TypedSharedBuffer<render::VertexShaderGeneral::UniformData>(1)};
    
    // renderer use
    render::Camera camera;
    camera.perspective(60, 16.f/9.f);
    shaderBRDF.vertexUniformBuffer()[0].inverse_view = camera.inverse_view();
    shaderBRDF.vertexUniformBuffer()[0].view = camera.view();
    shaderBRDF.vertexUniformBuffer()[0].projection = camera.projection();

    render::TypedSharedBuffer<render::InstanceData> instanceBuffer{1};
    render::Transform icosahedronTransform{instanceBuffer, 0};
    render::Mesh icosahedronMesh(icosahedronData.vertCount, icosahedronData.verts);
    icosahedronMesh.initElements(icosahedronData.elemCount, icosahedronData.indices);

    render::TypedSharedBuffer<render::FragmentShaderBRDF::UniformData> materialBuffer{1};
    materialBuffer[0].ambientLight = glm::vec3{0.1f, 0.1f, 0.1f};
    materialBuffer[0].lightColor = glm::vec3{1.f, 1.f, 1.f};
    materialBuffer[0].view_lightDirection = glm::normalize(glm::vec3{1.f, 1.f, 1.f});

    icosahedronTransform.position({0, 0, -10});
    icosahedronTransform.orientation(glm::quat({glm::radians(60.f), 0.f, 0.f}));
    icosahedronTransform.inverse();
    icosahedronTransform.matrix();

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderBRDF.Use();
        render::FragmentShaderBRDF::BindUniformBuffer(materialBuffer);
        icosahedronMesh.Draw(instanceBuffer);
        icosahedronTransform.orientation(glm::quat({0.f, glm::radians(0.2f), 0.f}) * icosahedronTransform.orientation());
        icosahedronTransform.inverse();
        icosahedronTransform.matrix();

        glfwSwapBuffers(window);
    }
    return 0;
}