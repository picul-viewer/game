#include "callback_allocator.h"
#include <lib/memory.h>

namespace resource_system {

void callback_allocator::create( )
{
	m_allocator.create( );
}

void callback_allocator::destroy( )
{
	m_allocator.destroy( );
}

struct callback_data
{
	uptr size;
	query_callback functor;
	u8 data[1];
};

query_callback_id callback_allocator::register_callback( query_callback const& in_functor, pointer const in_data, uptr const in_data_size )
{
	uptr const size = sizeof(uptr) + sizeof(query_callback) + in_data_size;

	callback_data* const data = m_allocator.allocate( size );

	data->size = size;
	data->functor = in_functor;
	memory::copy( data->data, in_data, in_data_size );

	return (query_callback_id)( (pointer)data - m_allocator.data( ) );
}

void callback_allocator::invoke_callback( query_callback_id const in_callback_id, queried_resources& in_queried_resources )
{
	callback_data* const data = m_allocator.data( ) + in_callback_id;

	data->functor( in_queried_resources, data->data );

	m_allocator.deallocate( data, data->size );
}

callback_allocator g_callback_allocator;

} // namespace resource_system