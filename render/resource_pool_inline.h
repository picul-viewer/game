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
	Resource* resource					= m_resource_allocator.allocate( sizeof(Resource) );
	resource->set_registry_pointer		( nullptr );

	return resource;
}

template<typename Resource>
Resource* resource_pool<Resource>::load_resource( weak_const_string const& in_path )
{
	u32 const path_hash					= in_path.hash( );

	u32* const id_ptr					= m_path_registry->find( *in_path.c_str( ), path_hash );

	if ( id_ptr == nullptr )
	{
		Resource* const resource		= m_resource_allocator.allocate( sizeof(Resource) );

		u32 const id					= (u32)( resource - (Resource*)m_resource_allocator.get_data( ) );
		pointer const registry_pointer	= m_path_registry->insert( *in_path.c_str( ), path_hash, id );
		
		create_resource_from_file		( resource, in_path );
		resource->set_registry_pointer	( registry_pointer );

		return resource;
	}
	else
	{
		Resource* const resource		= (Resource*)m_resource_allocator.get_data( ) + *id_ptr;
		resource->add_ref				( );

		return resource;
	}
}

template<typename Resource>
void resource_pool<Resource>::free_resource( Resource* in_resource )
{
	u32 const ref_count					= in_resource->release( );

	if ( ref_count == 0 )
	{
		pointer const registry_pointer	= in_resource->get_registry_pointer( );
		if ( registry_pointer )
			m_path_registry->remove_kv	( registry_pointer );

		m_resource_allocator.deallocate	( in_resource );
	}
}

template<typename Resource>
void resource_pool<Resource>::create_resource_from_file( Resource* out_resource, weak_const_string const& in_path )
{
	sys::file f							( in_path.c_str( ), sys::file::open_read );
	ASSERT								( f.is_valid( ) );
	uptr const size						= f.size( );

	pvoid const memory					= mem_allocator( ).allocate( size );

	f.read								( memory, size );
	f.close								( );

	binary_config cfg					( memory, size );
	out_resource->create				( cfg );

	mem_allocator( ).deallocate			( memory );
}

} // namespace render

#endif // #ifndef __render_resource_pool_inline_h_included_