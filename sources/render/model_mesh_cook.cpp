#include "model_mesh_cook.h"

#include <lib/allocator.h>
#include <lib/hash.h>
#include <lib/reader.h>
#include <lib/strings.h>
#include <resources/raw_data.h>
#include <resources/resources_path.h>

#include "model_mesh.h"
#include "mesh_cook.h"
#include "texture_cook.h"

namespace render {

model_mesh_cook::model_mesh_cook( pcstr const in_path ) :
	shared_resource_cook( create( in_path ) )
{ }

u32 model_mesh_cook::create( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	uptr const size = length + 1;

	m_length = length;
	strings::copy_n( m_path, in_path, size );

	return lib::hash32( in_path, length );
}

uptr model_mesh_cook::size( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	return sizeof(model_mesh_cook) + length + 1;
}

void model_mesh_cook::create_resource( )
{
	check_resource_exists( &model_mesh_cook::query_file );
}

void model_mesh_cook::query_file( model_mesh* const in_resource_ptr )
{
	m_result = in_resource_ptr;

	raw_data_cook* const cook = create_cook<raw_data_cook>( m_path );

	create_child_resources( callback_task<&model_mesh_cook::on_file_loaded>( ), cook );
}

void model_mesh_cook::on_file_loaded( queried_resources& in_queried )
{
	raw_data::ptr const data = in_queried.get_resource<raw_data::ptr>( );
	ASSERT( data != nullptr );

	lib::reader r( data->data( ), data->size( ) );

	// Read AABB.
	math::float3 const aabb_min = r.read<math::float3>( );
	math::float3 const aabb_max = r.read<math::float3>( );
	m_result->m_aabb.set_min_max( math::sse::vector( aabb_min ), math::sse::vector( aabb_max ) );

	task_info queries[3];
	u32 query_count = 0;

	// Mesh.
	{
		pcstr const path	= r.read_str( );
		mesh_cook* cook		= create_cook<mesh_cook>( get_resource_path( path ).c_str( ) );
		cook->fill_task_info( queries[query_count++] );
	}
	
	// Albedo texture.
	{
		pcstr const path	= r.read_str( );
		texture_cook* cook	= create_cook<texture_cook>( get_resource_path( path ).c_str( ) );
		cook->fill_task_info( queries[query_count++] );
	}

	// Metalness/roughness texture.
	{
		pcstr const path	= r.read_str( );
		texture_cook* cook	= create_cook<texture_cook>( get_resource_path( path ).c_str( ) );
		cook->fill_task_info( queries[query_count++] );
	}

	create_child_resources(
		queries, query_count,
		callback_task<&model_mesh_cook::on_subresources_loaded>( )
	);
}

void model_mesh_cook::on_subresources_loaded( queried_resources& in_queried )
{
	m_result->m_mesh = in_queried.get_resource<mesh_handle>( );

	m_result->m_albedo = in_queried.get_resource<texture_handle>( );
	m_result->m_metal_rough = in_queried.get_resource<texture_handle>( );

	finish( m_result );
}

} // namespace render