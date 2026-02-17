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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    stbi_set_flip_vertically_on_load(1);
    // renderer
    
    // camera setup
    render::Camera camera;
    camera.perspective(60, 1280, 720);
    camera.transform.position({0.f, 2.165063509f, 5.f});
    camera.transform.orientation(glm::quat(glm::vec3(glm::radians(-30.f), 0.f, 0.f)));
    camera.projection();
    camera.transform.matrix();
    camera.transform.inverse();
    camera.Use();

    // mesh setup - cube
    render::TypedSharedBuffer<render::InstanceData> cubeInstanceBuffer{1};
    render::Transform cubeTransform{cubeInstanceBuffer, &cubeInstanceBuffer.data()->model, &cubeInstanceBuffer.data()->inverse_model};
    render::Mesh cubeMesh(cubeData::vertCount, cubeData::verts);
    cubeMesh.initElements(cubeData::elemCount, cubeData::indices);
    cubeMesh.initNormals(cubeData::verts);
    cubeMesh.initUVs(cubeData::UVs);

    cubeTransform.position({0.0f, 0.0f, 0.f});
    cubeTransform.inverse();
    cubeTransform.matrix();

    // mesh setup - floor plane
    render::TypedSharedBuffer<render::InstanceData> floorInstanceBuffer{1};
    render::Transform floorTransform{floorInstanceBuffer, &floorInstanceBuffer.data()->model, &floorInstanceBuffer.data()->inverse_model};
    render::Mesh floorMesh(planeData::vertCount, planeData::verts);
    floorMesh.initElements(planeData::elemCount, planeData::indices);
    floorMesh.initUVs(planeData::UVs);

    floorTransform.position({0.f, -2.f, 0.f});
    floorTransform.orientation(glm::quat(glm::vec3(glm::radians(-90.f), 0.f, 0.f)));
    floorTransform.scale({10.f, 10.f, 10.f});
    floorTransform.inverse();
    floorTransform.matrix();

    // lighting setup
    render::FragmentShaderBRDF::Lighting lighting;
    lighting.uniformData.ambientLight = glm::vec3{0.15f, 0.15f, 0.15f};
    lighting.uniformData.lightColor = glm::vec3{1.f, 1.f, 1.f};
    glm::vec3 lightDir = glm::normalize(glm::vec3{1.f, 1.f, 1.f});
    lighting.uniformData.view_lightDirection = lightDir;
    lighting.uniformData.world_lightDirection = lightDir;
    
    // shadow setup
    render::ShadowMapPass shadow{2048};
    lighting.uniformData.shadowBias = shadow.bias;
    lighting.uniformData.shadowEnabled = true;

    // texture setup
    render::Image bricksAlbedoImg = render::Image::FromFile(render::TexCompType::UNSIGNED_BYTE, "./renderer/demo/assets/bricks/Bricks101_1K-PNG_Color.png", 3);
    render::Texture2D bricksAlbedo(bricksAlbedoImg, 1, GL_RGB8);
    
    render::Image bricksNormalImg = render::Image::FromFile(render::TexCompType::UNSIGNED_BYTE, "./renderer/demo/assets/bricks/Bricks101_1K-PNG_NormalGL.png", 3);
    render::Texture2D bricksNormal(bricksNormalImg, 1, GL_RGB8);

    render::Image bricksRoughnessImg = render::Image::FromFile(render::TexCompType::UNSIGNED_BYTE, "./renderer/demo/assets/bricks/Bricks101_1K-PNG_Roughness.png", 1);
    render::Texture2D bricksRoughness(bricksRoughnessImg, 1, GL_R8);

    render::Image bricksAOImg = render::Image::FromFile(render::TexCompType::UNSIGNED_BYTE, "./renderer/demo/assets/bricks/Bricks101_1K-PNG_AmbientOcclusion.png", 1);
    render::Texture2D bricksAO(bricksAOImg, 1, GL_R8);

    // material setup
    render::FragmentShaderBRDF::Material material;
    material.uniformData.metallic_mod = 0.1f;
    material.textures[render::FragmentShaderBRDF::ALBEDO_MAP_UNIT] = bricksAlbedo;
    material.textures[render::FragmentShaderBRDF::NORMAL_MAP_UNIT] = bricksNormal;
    material.textures[render::FragmentShaderBRDF::ROUGHNESS_MAP_UNIT] = bricksRoughness;
    material.textures[render::FragmentShaderBRDF::AMBIENT_OCCLUSION_MAP_UNIT] = bricksAO;

    // floor material (simple white)
    render::FragmentShaderBRDF::Material floorMaterial;
    floorMaterial.uniformData.color_mod = {0.8f, 0.8f, 0.8f, 1.0f};
    floorMaterial.uniformData.roughness_mod = 0.9f;
    floorMaterial.uniformData.metallic_mod = 0.0f;

    // shader creation
    render::ShaderProgramBRDF shaderBRDF;

    while(!glfwWindowShouldClose(window))
    {
        // ---- PASS 1: Shadow depth pass ----
        shadow.BeginPass(lightDir, glm::vec3(0.f, 0.f, -2.5f), 15.0f);
        
        glClear(GL_DEPTH_BUFFER_BIT); // clears the shadow FBO
        // Render all shadow-casting geometry (depth only)
        cubeMesh.Draw(cubeInstanceBuffer);
        floorMesh.Draw(floorInstanceBuffer);
        
        
        // ---- PASS 2: Main rendering pass ----
        int wWidth, wHeight;
        glfwGetWindowSize(window, &wWidth, &wHeight);
        camera.resolution = {wWidth, wHeight};
        camera.perspective(camera.fov, wWidth, wHeight);

        render::Framebuffer::Unbind(); // bind default framebuffer
        glViewport(0, 0, wWidth, wHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the screen
        shaderBRDF.Use();
        camera.Use();
        lighting.Use();
        shadow.Use();

        // Draw cube with brick material
        material.Use();
        cubeMesh.Draw(cubeInstanceBuffer);

        // Draw floor with simple material
        floorMaterial.Use();
        floorMesh.Draw(floorInstanceBuffer);

        // update
        glfwPollEvents();
        cubeTransform.orientation(glm::quat({0.f, glm::radians(0.2f), 0.f}) * cubeTransform.orientation());
        cubeTransform.inverse();
        cubeTransform.matrix();

        glfwSwapBuffers(window);
    }

    return 0;
}