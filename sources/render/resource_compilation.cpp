#include "resource_compilation.h"
#include <math/vector.h>
#include <math/matrix.h>

namespace render {

void model_mesh_serialize( utils::config_reader_node const& in_cfg, lib::dynamic_writer& in_writer )
{
	{
		utils::config_reader_node const& n = in_cfg["aabb_min"];
		ASSERT( n.valid( ) );

		math::float3 const& value = math::float3(
			(float)n( 0 ).as_float( ),
			(float)n( 1 ).as_float( ),
			(float)n( 2 ).as_float( )
		);
		in_writer.write( value );
	}

	{
		utils::config_reader_node const& n = in_cfg["aabb_max"];
		ASSERT( n.valid( ) );

		math::float3 const& value = math::float3(
			(float)n( 0 ).as_float( ),
			(float)n( 1 ).as_float( ),
			(float)n( 2 ).as_float( )
		);
		in_writer.write( value );
	}

	{
		utils::config_reader_node const& n = in_cfg["mesh"];
		ASSERT( n.valid( ) );

		in_writer.write_str( n.as_string( ) );
	}

	{
		utils::config_reader_node const& n = in_cfg["diffuse"];
		ASSERT( n.valid( ) );

		in_writer.write_str( n.as_string( ) );
	}

	{
		utils::config_reader_node const& n = in_cfg["specular"];
		ASSERT( n.valid( ) );

		in_writer.write_str( n.as_string( ) );
	}
}

void object_mesh_serialize( utils::config_reader_node const& in_cfg, lib::dynamic_writer& in_writer )
{
	{
		utils::config_reader_node const& n = in_cfg["model"];
		ASSERT( n.valid( ) );

		in_writer.write_str( n.as_string( ) );
	}

	{
		utils::config_reader_node const& n = in_cfg["transform"];
		ASSERT( n.valid( ) );

		ASSERT( n( 0 ).valid( ) );
		ASSERT( n( 1 ).valid( ) );
		ASSERT( n( 2 ).valid( ) );

		math::float4x3 const& value = math::float4x3( 
			math::float4(
				(float)n( 0 )( 0 ).as_float( ),
				(float)n( 0 )( 1 ).as_float( ),
				(float)n( 0 )( 2 ).as_float( ),
				(float)n( 0 )( 3 ).as_float( ) ),
			math::float4(
				(float)n( 1 )( 0 ).as_float( ),
				(float)n( 1 )( 1 ).as_float( ),
				(float)n( 1 )( 2 ).as_float( ),
				(float)n( 1 )( 3 ).as_float( ) ),
			math::float4(
				(float)n( 2 )( 0 ).as_float( ),
				(float)n( 2 )( 1 ).as_float( ),
				(float)n( 2 )( 2 ).as_float( ),
				(float)n( 2 )( 3 ).as_float( ) )
		);

		in_writer.write( value );
	}
}

void scene_serialize( utils::config_reader_node const& in_cfg, lib::dynamic_writer& in_writer )
{
	{
		utils::config_reader_node const& n = in_cfg["static_meshes"];
		ASSERT( n.valid( ) );

		uptr const count = n.size( );
		in_writer.write( (u32)count );

		for ( uptr i = 0, s = count; i < s; ++i )
		{
			object_mesh_serialize( n[i], in_writer );
		}
	}
}

} // namespace render