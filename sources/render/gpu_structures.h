#ifndef GUARD_RENDER_GPU_STRUCTURES_H_INCLUDED
#define GUARD_RENDER_GPU_STRUCTURES_H_INCLUDED

#include <types.h>
#include <math/vector.h>
#include <math/matrix.h>

namespace render {
namespace gpu {

using uint = u32;
using uint2 = math::u32x2;
using uint3 = math::u32x3;
using uint4 = math::u32x4;
using half = math::half;
using half2 = math::half2;
using half3 = math::half3;
using half4 = math::half4;
using float2 = math::float2;
using float3 = math::float3;
using float4 = math::float4;
using float2x2 = math::float2x2;
using float3x3 = math::float3x3;
using float3x4 = math::float4x3;
using float4x4 = math::float4x4;

#include <../resources/sources/shaders/structures.h>

} // namespace gpu
} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_STRUCTURES_H_INCLUDED