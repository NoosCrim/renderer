#pragma once
#include <vector>
#include <glm/glm.hpp>
constexpr glm::vec4 HSV_to_RGB(float H, float S, float V)
{
    int wheel_segment = ((int)(H/60));
    glm::vec4 samples[6] = 
    {
        {1,0,0,1},
        {1,1,0,1},
        {0,1,0,1},
        {0,1,1,1},
        {0,0,1,1},
        {1,0,1,1}
    };
    float A = wheel_segment * 60, B = (wheel_segment+1) * 60;
    float wA = (B - H)/60, wB = (H - A)/60;
    glm::vec4 col = wA * samples[wheel_segment%6] + wB *samples[(wheel_segment+1)%6];
    col = col * S + glm::vec4(1,1,1,1) * (1.0f-S);
    return col * V;
}
namespace icosahedronData
{
    static constexpr float f = (1 + 2.236067977f) / 2;
    static constexpr GLuint vertCount = 12, elemCount = 60;
    static constexpr glm::vec3 verts[vertCount] = 
    {    
        {-1, f, 0}, 
        {1, f, 0}, 
        {-1, -f, 0}, 
        {1, -f, 0}, 
        {0, -1, f}, 
        {0, 1, f}, 
        {0, -1, -f}, 
        {0, 1, -f}, 
        {f, 0, -1}, 
        {f, 0, 1}, 
        {-f, 0, -1}, 
        {-f, 0, 1}
    };
    static constexpr GLuint indices[elemCount] = 
    {
        0 , 11, 5 , 0 , 5 , 1 , 0 , 1 , 7 , 0 , 7 , 10, 0 , 10, 11, 
        11, 10, 2 , 5 , 11, 4 , 1 , 5 , 9 , 7 , 1 , 8 , 10, 7 , 6 , 
        3 , 9 , 4 , 3 , 4 , 2 , 3 , 2 , 6 , 3 , 6 , 8 , 3 , 8 , 9 , 
        9 , 8 , 1 , 4 , 9 , 5 , 2 , 4 , 11, 6 , 2 , 10, 8 , 6 , 7
    };
}
namespace planeData
{
    static constexpr GLuint vertCount = 4, elemCount = 6;
    static constexpr glm::vec3 verts[vertCount] = 
    {    
        {-1.f, -1.f, 0.f},
        { 1.f, -1.f, 0.f},
        { 1.f,  1.f, 0.f},
        {-1.f,  1.f, 0.f}
    };
    static constexpr glm::vec2 UVs[vertCount] = 
    {
        {0.f, 0.f},
        {1.f, 0.f},
        {1.f, 1.f},
        {0.f, 1.f}
    };
    static constexpr GLuint indices[elemCount] = 
    {
        0,1,2,
        0,2,3
    };
}
namespace cubeData
{
    static constexpr GLuint vertCount = 12, elemCount = 36;
    static constexpr glm::vec3 verts[vertCount] = 
    {    
        {-1.f, -1.f, -1.f},
        { 1.f, -1.f, -1.f},
        { 1.f, -1.f,  1.f},
        {-1.f, -1.f,  1.f},
        {-1.f,  1.f, -1.f},
        { 1.f,  1.f, -1.f},
        { 1.f,  1.f,  1.f},
        {-1.f,  1.f,  1.f},

        {-1.f, -1.f,  1.f},
        { 1.f, -1.f,  1.f},
        {-1.f,  1.f,  1.f},
        { 1.f,  1.f,  1.f}
    };
    static constexpr GLuint indices[elemCount] = 
    {
        0,1,9, 0,9,8,
        4,11,5, 4,10,11,

        0,5,1, 0,4,5,
        1,6,2, 1,5,6,
        2,7,3, 2,6,7,
        3,4,0, 3,7,4
    };
    static constexpr glm::vec2 UVs[vertCount] = 
    {
        {1,1},
        {0,1},
        {1,1},
        {0,1},
        
        {1,0},
        {0,0},
        {1,0},
        {0,0},

        {1,0},
        {0,0},
        {1,1},
        {0,1}
    };
}