#include "headers/builtin_shader.hpp"
namespace render
{
    bool VertexShaderGeneral::_uniformBufferInitialized = false;
    TypedSharedBuffer<VertexShaderGeneral::UniformData> VertexShaderGeneral::_uniformBuffer{};
}