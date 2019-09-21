#include "config_compiler_common.h"

namespace resource_compiler {

math::float2 read_float2( utils::config_reader_node const& in_cfg )
{
	ASSERT( in_cfg.valid( ) );

	math::float2 const& value = math::float2(
		(float)in_cfg( 0 ).as_float( ),
		(float)in_cfg( 1 ).as_float( )
	);

	return value;
}

math::float3 read_float3( utils::config_reader_node const& in_cfg )
{
	ASSERT( in_cfg.valid( ) );

	math::float3 const& value = math::float3(
		(float)in_cfg( 0 ).as_float( ),
		(float)in_cfg( 1 ).as_float( ),
		(float)in_cfg( 2 ).as_float( )
	);

	return value;
}

math::float4 read_float4( utils::config_reader_node const& in_cfg )
{
	ASSERT( in_cfg.valid( ) );

	math::float4 const& value = math::float4(
		(float)in_cfg( 0 ).as_float( ),
		(float)in_cfg( 1 ).as_float( ),
		(float)in_cfg( 2 ).as_float( ),
		(float)in_cfg( 3 ).as_float( )
	);

	return value;
}

math::float2x2 read_float2x2( utils::config_reader_node const& in_cfg )
{
	ASSERT( in_cfg.valid( ) );

	math::float2x2 const& value = math::float2x2(
		read_float2( in_cfg( 0 ) ),
		read_float2( in_cfg( 1 ) )
	);

	return value;
}

math::float3x3 read_float3x3( utils::config_reader_node const& in_cfg )
{
	ASSERT( in_cfg.valid( ) );

	math::float3x3 const& value = math::float3x3(
		read_float3( in_cfg( 0 ) ),
		read_float3( in_cfg( 1 ) ),
		read_float3( in_cfg( 2 ) )
	);

	return value;
}

math::float4x3 read_float4x3( utils::config_reader_node const& in_cfg )
{
	ASSERT( in_cfg.valid( ) );

	math::float4x3 const& value = math::float4x3(
		read_float4( in_cfg( 0 ) ),
		read_float4( in_cfg( 1 ) ),
		read_float4( in_cfg( 2 ) )
	);

	return value;
}

math::float4x4 read_float4x4( utils::config_reader_node const& in_cfg )
{
	ASSERT( in_cfg.valid( ) );

	math::float4x4 const& value = math::float4x4(
		read_float4( in_cfg( 0 ) ),
		read_float4( in_cfg( 1 ) ),
		read_float4( in_cfg( 2 ) ),
		read_float4( in_cfg( 3 ) )
	);

	return value;
}

} // namespace resource_compiler