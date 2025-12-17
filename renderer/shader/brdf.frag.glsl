#version 460

#include "general_shared.glsl"

layout(std140, binding = 1) uniform _brdfUniforms
{
    vec3 ambientLight;
    uint activeTextureBitfield;
    vec3 lightColor;
    float _pad1;
    vec3 view_lightDirection;
};

layout(binding = 2) uniform sampler2D albedo_map;
layout(binding = 3) uniform sampler2D Roughness_map;
layout(binding = 4) uniform sampler2D metallic_map;
layout(binding = 5) uniform sampler2D normal_map;
layout(binding = 6) uniform sampler2D ambient_occlusion_map;

bool isMapEnabled(uint map)
{
    return ((activeTextureBitfield >> map) & 1u) == 1u;
}

const uint ALBEDO_MAP = 0;
const uint ROUGHNESS_MAP = ALBEDO_MAP + 1;
const uint METALLIC_MAP = ROUGHNESS_MAP + 1;
const uint NORMAL_MAP = METALLIC_MAP + 1;
const uint AMBIENT_OCCLUSION_MAP = NORMAL_MAP + 1;
const uint NEXT_MAP = AMBIENT_OCCLUSION_MAP + 1;

bool ALBEDO_MAP_ENABLED = isMapEnabled(ALBEDO_MAP);
bool ROUGHNESS_MAP_ENABLED = isMapEnabled(ROUGHNESS_MAP);
bool METALLIC_MAP_ENABLED = isMapEnabled(METALLIC_MAP);
bool NORMAL_MAP_ENABLED = isMapEnabled(NORMAL_MAP);
bool AMBIENT_OCCLUSION_MAP_ENABLED = isMapEnabled(AMBIENT_OCCLUSION_MAP);


in vec4 frag_view_pos;
in vec4 frag_pos;
in vec4 frag_color;
in vec2 frag_uv;
in vec3 frag_view_normal;
in vec3 frag_view_tangent;
in vec3 frag_view_bitangent;

out vec4 out_color;

void main()
{
    vec4 color = frag_color;
    vec3 view_normal, view_tangent, view_bitangent;

    vec3 view_pos_dx = dFdx(vec3(frag_view_pos));
    vec3 view_pos_dy = dFdy(vec3(frag_view_pos));
    vec2 uv_dx = dFdx(frag_uv);
    vec2 uv_dy = dFdy(frag_uv);

    if(NORMAL_ENABLED)
    {
        view_normal = normalize(frag_view_normal);
    }
    else
        view_normal = normalize(cross(view_pos_dx, view_pos_dy));

    if(TANGENT_ENABLED && NORMAL_ENABLED)
    {
        view_tangent = normalize(frag_view_tangent);
        view_bitangent = normalize(frag_view_bitangent);
    }
    else
    {
        view_tangent = normalize(cross(view_pos_dy, view_normal) * uv_dx.x + cross(view_normal, view_pos_dx) * uv_dy.x);
        view_bitangent = cross(view_normal, view_tangent);
    }
    mat3 TBN = mat3(view_tangent, view_bitangent, view_normal);

    vec3 dL = clamp(dot(view_normal, view_lightDirection), 0.f, 1.f) * lightColor;
    vec3 aL = ambientLight;
    out_color = vec4((dL + aL) * vec3(color), color.a); // simple lambertian for now
}