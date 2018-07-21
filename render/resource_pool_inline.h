#ifndef __render_resource_pool_inline_h_included_
#define __render_resource_pool_inline_h_included_

#include <lib/alloc_string.h>

namespace render {

template<typename Resource>
resource_pool<Resource>::resource_pool( )
{ }

template<typename Resource>
void resource_pool<Resource>::create( uptr in_hash_map_table_size )
{
	m_string_pool.create( );
	m_pool_data_allocator.create( );

	new ( &m_pool_data ) pool_data( in_hash_map_table_size, aligned_mem_allocator<Cache_Line>( ), m_pool_data_allocator );
}

template<typename Resource>
void resource_pool<Resource>::destroy( )
{
	m_pool_data.for_each( []( weak_const_string, Resource& resource )
	{
		resource.destroy( );
	} );

	m_pool_data.clear( );
	
	m_string_pool.destroy( );
	m_pool_data_allocator.destroy( );
}

template<typename Resource>
Resource* resource_pool<Resource>::get( weak_const_string const& in_filename )
{
	u32 filename_hash	= in_filename.hash( );

	Resource* resource	= m_pool_data.find( in_filename, filename_hash );

	if ( resource == nullptr )
	{
		alloc_string	resource_path( m_string_pool, in_filename );

		auto inserted	= m_pool_data.insert( weak_const_string( resource_path ), filename_hash, Resource( ) );
		resource		= &inserted->value( );
		
		Resource::load	( resource, in_filename );
	}
	
	return resource;
}

} // namespace render

#endif // #ifndef __render_resource_pool_inline_h_included_