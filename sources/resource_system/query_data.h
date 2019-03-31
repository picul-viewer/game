#ifndef GUARD_RESOURCE_SYSTEM_QUERY_DATA_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_QUERY_DATA_H_INCLUDED

#include <types.h>
#include <system/interlocked.h>
#include "resource_system_types.h"

namespace resource_system {

struct query_data
{
	mt_u32 waiting_factor;
	u32 base_size;
	u32 index;
	resource_type type;
	u32 requested_queries_count;
	u32 dependent_queries_count;
	
	bool is_fs_query( ) const;

	pointer cook_data( ) const;
	
	u32* dependent_queries( );
	u32 const* dependent_queries( ) const;

	u32* requested_queries( );
	u32 const* requested_queries( ) const;

	query_callback_id& callback_id( ) const;

	static uptr calculate_base_size( uptr const in_cook_data_size );
	static uptr calculate_size( uptr const in_cook_data_size, u32 const in_dependent_queries_count, u32 const in_requested_queries_count );
	uptr size( ) const;
};

struct file_header
{
	mt_u32 ref_count;
	u32 size;
	u8 data[1];

	static uptr calculate_size( uptr const in_file_size );
};

struct request_header
{
	mt_u32 ref_count;
	u32 finish_query_offset;
};

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_QUERY_DATA_H_INCLUDED