#pragma once
#include <vector>
#include <glm/glm.hpp>
struct __icosahedronData
{
    private:
        static constexpr float f = (1 + 2.236067977f) / 2;
    public:
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
} icosahedronData;