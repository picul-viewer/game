#ifndef __render_resource_pool_inline_h_included_
#define __render_resource_pool_inline_h_included_

#include "render_core.h"
#include <fs/fs_core.h>

namespace render {

template<typename T>
resource_pool<T>::resource_pool( ) :
	m_resource_path( "" )
{ }

template<typename T>
void resource_pool<T>::set_resource_path( pcstr in_path )
{
	m_resource_path = in_path;
}

template<typename T>
static void create_resource_impl( pointer in_data, uptr in_size, T* in_resource )
{
	in_resource->create( in_data, in_size );
	mem_allocator( ).deallocate( in_data );
}

template<typename T>
T* resource_pool<T>::create_resource( weak_const_string const& in_filename )
{
	fixed_string<256> path	= m_resource_path;
	path					+= in_filename;

	pool_data::iterator it	= m_pool_data.find( path );

	if ( it != m_pool_data.end( ) )
		it->second.add_ref( );
	else
	{
		alloc_string		resource_path( m_string_pool, in_filename );
		it					= m_pool_data.emplace( make_pair( weak_const_string( resource_path ), T( ) ) ).first;
		
		// Read data from file with callback
		fs::core::read_file( in_filename.c_str( ), &create_resource_impl<T>, &it->second );
	}
	
	return &it->second;
}

template<typename T>
void resource_pool<T>::free_resource( T* in_resource )
{
	in_resource->release( );
}

} // namespace render

#endif // #ifndef __render_resource_pool_inline_h_included_