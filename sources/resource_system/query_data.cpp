#include "query_data.h"
#include <math/math_common.h>
#include "resource_types.h"

namespace resource_system {

bool query_data::is_fs_query( ) const
{
	return ( type == resource_type_file ) || ( type == resource_type_request );
}

pointer query_data::cook_data( ) const
{
	return (pointer)( this + 1 );
}

u32* query_data::dependent_queries( )
{
	return (pointer)this + (uptr)base_size;
}

u32 const* query_data::dependent_queries( ) const
{
	return (pointer)this + (uptr)base_size;
}

u32* query_data::requested_queries( )
{
	return (pointer)this + (uptr)base_size + (uptr)dependent_queries_count * sizeof(u32);
}

u32 const* query_data::requested_queries( ) const
{
	return (pointer)this + (uptr)base_size + (uptr)dependent_queries_count * sizeof(u32);
}

query_callback_id& query_data::callback_id( ) const
{
	// Note: only relevant for finish request query data.
	return *(query_callback_id*)( (pointer)this + size( ) );
}

uptr query_data::calculate_base_size( uptr const in_cook_data_size )
{
	return
		sizeof(query_data) +
		math::align_up( in_cook_data_size, 8u );
}

uptr query_data::calculate_size( uptr const in_cook_data_size, u32 const in_dependent_queries_count, u32 const in_requested_queries_count )
{
	return
		calculate_base_size( in_cook_data_size ) +
		sizeof(u32) * in_dependent_queries_count +
		sizeof(u32) * in_requested_queries_count;
}

uptr query_data::size( ) const
{
	return
		base_size +
		sizeof(u32) * dependent_queries_count +
		sizeof(u32) * requested_queries_count;
}


uptr file_header::calculate_size( uptr const in_file_size )
{
	return 2 * sizeof(u32) + in_file_size;
}

} // namespace resource_system