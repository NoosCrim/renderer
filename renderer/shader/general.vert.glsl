#version 460

#include "general_shared.glsl"

layout(location = POS_IDX) in vec3 pos;
layout(location = COLOR_IDX) in vec4 color;
layout(location = UV_IDX) in vec2 uv;
layout(location = NORMAL_IDX) in vec3 normal;
layout(location = TANGENT_IDX) in vec3 tangent;
layout(location = INSTANCE_TRANSFORM_IDX) in mat4 model;
layout(location = INSTANCE_INVERSE_TRANSFORM_IDX) in mat4 inverse_model;

mat4 viewModel = view * model;

out vec4 frag_view_pos;
out vec4 frag_pos;
out vec4 frag_color;
out vec2 frag_uv;
out vec3 frag_view_normal;
out vec3 frag_view_tangent;
out vec3 frag_view_bitangent;



layout(binding = 1) uniform sampler2D AlbedoMap;
layout(binding = 2) uniform sampler2D NormalMap;
layout(binding = 3) uniform sampler2D ORMMap; // Occlusion, Roughness, Metallic

void main()
{
    frag_view_pos = viewModel * vec4(pos, 1.f);
    gl_Position = projection * frag_view_pos;
    frag_pos = gl_Position;
    frag_color = COLOR_ENABLED ? color : vec4(1.0f);
    frag_uv = uv;
    
    if(NORMAL_ENABLED)
        frag_view_normal = mat3(transpose(inverse_model * inverse_view)) * normal;
    if(NORMAL_ENABLED && TANGENT_ENABLED)
    {
        frag_view_tangent = mat3(viewModel) * tangent;
        frag_view_bitangent = cross(frag_view_normal, frag_view_tangent);
    }
}

