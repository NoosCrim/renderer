#version 460

#include "shadow_map_shared.glsl"

layout(location = 0) in vec3 pos;
layout(location = 5) in mat4 model;

layout(location = 0) uniform uint activeAttribBitfield; // needed for Mesh::Draw compatibility

void main()
{
    gl_Position = lightViewProjection * model * vec4(pos, 1.0);
}
