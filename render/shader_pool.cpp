#include "shader_pool.h"
#include "input_layout.h"
#include <fs/fs_core.h>

#ifdef DEBUG
#include <fstream>

static pcstr missed_shaders_list_path	= "missing_shaders.txt";
#endif // #ifdef DEBUG

namespace render {

static pcstr shader_root_path	= "resources/shaders/";

static inline fixed_string<256> get_shader_path( pcstr in_filename, u32 in_configuration )
{
	fixed_string<256> result	= shader_root_path;
	result						+= in_filename;
	uptr length					= result.length( );

	ASSERT						( length + 1u + 32u + 1u <= 512 );

	result[length]				= '/';

	// To string
	for ( u32 i = 0u; i < 32u; ++i )
	{
		result[++length]		= '0' + ( in_configuration & 1u );
		in_configuration		>>= 1u;
	}

	result[++length]			= '\0';

	return						result;
}

static inline shader_pool::shader_name_string get_shader_name( pcstr in_filename, u32 in_configuration )
{
	shader_pool::shader_name_string result	= in_filename;
	uptr length								= result.length( );

	ASSERT									( length + 4u < c_shader_name_max_length );

	for ( u32 i = 0u; i < 8u; ++i )
	{
		result[++length]		= (char)( 'a' + ( in_configuration & 0x0F ) );
		in_configuration		>>= 4u;
	}

	result[++length]			= '\0';

	return						result;
}

static void create_vertex_impl( pointer in_data, uptr in_size, vertex_shader* in_resource, vertex_type in_vertex )
{
	D3D11_INPUT_ELEMENT_DESC* const	desc		= get_vertex_type_desc		( in_vertex );
	u32 const						desc_size	= get_vertex_type_desc_size	( in_vertex );

	in_resource->create( in_data, in_size, desc, desc_size );

	mem_allocator( ).deallocate( in_data );
}

vertex_shader* shader_pool::create_vertex( pcstr in_filename, u32 in_configuration )
{
	shader_name_string name				= get_shader_name( in_filename, in_configuration );

	auto it								= m_vertex_shader_data.find( name );

	if ( it != m_vertex_shader_data.end( ) )
		it->second.add_ref				( );
	else
	{
		it								= m_vertex_shader_data.emplace( make_pair( name, vertex_shader( ) ) ).first;
		
		vertex_type const vertex		= (vertex_type)( in_configuration & 0xFF );
		
		// Read data from file with callback
		fixed_string<256> const& path	= get_shader_path( in_filename, in_configuration );

#ifdef DEBUG
		if ( !fs::file_exists( path.c_str( ) ) )
		{
			std::fstream				log( missed_shaders_list_path, std::ios_base::app );
			log							<< in_filename << std::endl << in_configuration << std::endl;
			log.close					( );
		}
		else
#endif // #ifdef DEBUG
		fs::core::read_file				( path.c_str( ), &create_vertex_impl, &it->second, vertex );
	}
	
	return &it->second;
}

template<typename T>
static void create_shader_impl( pointer in_data, uptr in_size, T* in_resource )
{
	in_resource->create( in_data, in_size );
	mem_allocator( ).deallocate( in_data );
}

template<typename T>
T* shader_pool::create_shader( pcstr in_filename, u32 in_configuration, shader_map<T>& in_collection )
{
	shader_name_string name				= get_shader_name( in_filename, in_configuration );

	auto it								= in_collection.find( name );

	if ( it != in_collection.end( ) )
		it->second.add_ref				( );
	else
	{
		it								= in_collection.emplace( make_pair( name, T( ) ) ).first;
		
		// Read data from file with callback
		fixed_string<256> const& path	= get_shader_path( in_filename, in_configuration );

#ifdef DEBUG
		if ( !fs::file_exists( path.c_str( ) ) )
		{
			std::fstream				log( missed_shaders_list_path, std::ios_base::app );
			log							<< in_filename << std::endl << in_configuration << std::endl;
			log.close					( );
		}
		else
#endif // #ifdef DEBUG
		fs::core::read_file				( path.c_str( ), &create_shader_impl<T>, &it->second );
	}
	
	return &it->second;
}

pixel_shader* shader_pool::create_pixel( pcstr in_filename, u32 in_configuration )
{
	return create_shader( in_filename, in_configuration, m_pixel_shader_data );
}

geometry_shader* shader_pool::create_geometry( pcstr in_filename, u32 in_configuration )
{
	return create_shader( in_filename, in_configuration, m_geometry_shader_data );
}

hull_shader* shader_pool::create_hull( pcstr in_filename, u32 in_configuration )
{
	return create_shader( in_filename, in_configuration, m_hull_shader_data );
}

domain_shader* shader_pool::create_domain( pcstr in_filename, u32 in_configuration )
{
	return create_shader( in_filename, in_configuration, m_domain_shader_data );
}

compute_shader* shader_pool::create_compute( pcstr in_filename, u32 in_configuration )
{
	return create_shader( in_filename, in_configuration, m_compute_shader_data );
}

void shader_pool::free_vertex( vertex_shader* in_resource )
{
	in_resource->release( );
}

void shader_pool::free_pixel( pixel_shader* in_resource )
{
	in_resource->release( );
}

void shader_pool::free_geometry( geometry_shader* in_resource )
{
	in_resource->release( );
}

void shader_pool::free_hull( hull_shader* in_resource )
{
	in_resource->release( );
}

void shader_pool::free_domain( domain_shader* in_resource )
{
	in_resource->release( );
}

void shader_pool::free_compute( compute_shader* in_resource )
{
	in_resource->release( );
}

} // namespace render