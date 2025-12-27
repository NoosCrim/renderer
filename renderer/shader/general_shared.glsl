layout(std140, binding = 0) uniform _generalUniforms
{
    mat4 inverse_view;
    mat4 view;
    mat4 projection;
    // vec2 resolution;
    // vec2 time_deltaTime;
};
layout(location = 0) uniform uint activeAttribBitfield;

bool isAttribEnabled(uint idx)
{
    return ((activeAttribBitfield >> idx) & 1u) == 1u;
}

const uint POS_IDX = 0;
const uint COLOR_IDX = 1 + POS_IDX;
const uint UV_IDX = 1 + COLOR_IDX;
const uint NORMAL_IDX = 1 + UV_IDX;
const uint TANGENT_IDX = 1 + NORMAL_IDX;
const uint INSTANCE_TRANSFORM_IDX = 1 + TANGENT_IDX;
const uint INSTANCE_INVERSE_TRANSFORM_IDX = 4 + INSTANCE_TRANSFORM_IDX;
const uint NEXT_IDX = 4 + INSTANCE_INVERSE_TRANSFORM_IDX;


// we assume POS, TRANSFORM and INVERSE_TRANSFORM always enabled
bool COLOR_ENABLED = isAttribEnabled(COLOR_IDX);
bool UV_ENABLED = isAttribEnabled(UV_IDX);
bool NORMAL_ENABLED = isAttribEnabled(NORMAL_IDX);
bool TANGENT_ENABLED = isAttribEnabled(TANGENT_IDX);