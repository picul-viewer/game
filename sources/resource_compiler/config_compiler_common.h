#ifndef GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_COMMON_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_COMMON_H_INCLUDED

#include <types.h>
#include <math/vector.h>
#include <math/matrix.h>
#include <utils/config_reader.h>

namespace resource_compiler {

math::float2 read_float2( utils::config_reader_node const& in_cfg );
math::float3 read_float3( utils::config_reader_node const& in_cfg );
math::float4 read_float4( utils::config_reader_node const& in_cfg );

math::float2x2 read_float2x2( utils::config_reader_node const& in_cfg );
math::float3x3 read_float3x3( utils::config_reader_node const& in_cfg );
math::float4x3 read_float4x3( utils::config_reader_node const& in_cfg );
math::float4x4 read_float4x4( utils::config_reader_node const& in_cfg );

} // namespace resource_compiler

#endif // #ifndef GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_COMMON_H_INCLUDED