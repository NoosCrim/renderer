// Shadow mapping shared definitions
// Include this in any shader that needs shadow mapping support

const uint SHADOW_UBO_BINDING = 15u;
const uint SHADOW_TEXTURE_UNIT = 15u;

layout(std140, binding = SHADOW_UBO_BINDING) uniform _shadowUniforms
{
    mat4 lightViewProjection;
};

layout(binding = SHADOW_TEXTURE_UNIT) uniform sampler2D shadow_map;

float CalculateShadow(vec4 worldPos, float shadowBias)
{   
    vec4 lightSpacePos = lightViewProjection * worldPos;
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0,1]
    
    if (projCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float shadow = ((currentDepth - shadowBias) > closestDepth) ? 1.0 : 0.0;
    return shadow;
}
