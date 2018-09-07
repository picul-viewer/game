#ifndef __render_resource_pool_inline_h_included_
#define __render_resource_pool_inline_h_included_

#include <lib/alloc_string.h>

namespace render {

template<typename Resource>
resource_pool<Resource>::resource_pool( )
{ }

template<typename Resource>
void resource_pool<Resource>::create( resource_path_registry& in_resource_path_registry )
{
	m_resource_allocator.create( );
	m_path_registry = &in_resource_path_registry;
}

template<typename Resource>
void resource_pool<Resource>::destroy( )
{
	m_resource_allocator.destroy( );
}

template<typename Resource>
Resource* resource_pool<Resource>::create_resource( )
{
	return m_resource_allocator.allocate( sizeof(Resource) );
}

template<typename Resource>
void resource_pool<Resource>::destroy_resource( Resource* in_resource )
{
	m_resource_allocator.deallocate( in_resource );
}

template<typename Resource>
Resource* resource_pool<Resource>::get_resource( weak_const_string const& in_path )
{
	u32 const path_hash				= in_path.hash( );

	u32* const id_ptr				= m_path_registry->find( *in_path.c_str( ), path_hash );

	if ( id_ptr == nullptr )
	{
		Resource* const resource	= create_resource( );

		u32 const id				= (u32)( resource - (Resource*)m_resource_allocator.get_data( ) );
		m_path_registry->insert		( *in_path.c_str( ), path_hash, id );
		
		load_resource				( resource, in_path );

		return resource;
	}
	else
		return (Resource*)m_resource_allocator.get_data( ) + *id_ptr;
}

template<typename Resource>
void resource_pool<Resource>::load_resource( Resource* out_resource, weak_const_string const& in_path )
{
	sys::file f						( in_path.c_str( ), sys::file::open_read );
	ASSERT							( f.is_valid( ) );
	uptr const size					= f.size( );

	pvoid const memory				= mem_allocator( ).allocate( size );

	f.read							( memory, size );
	f.close							( );

	binary_config cfg				( memory, size );
	out_resource->create			( cfg );

	mem_allocator( ).deallocate		( memory );
}

} // namespace render

#endif // #ifndef __render_resource_pool_inline_h_included_