#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPES_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_TYPES_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"

namespace resource_system {

#define RESOURCE_SYSTEM_FIRST_RESOURCE_TYPE( system ) ( (::resource_system::resource_type)::resource_system::resource_type_group_ ## system << 24u )

enum resource_type_groups : u8
{
	resource_type_group_render,

	resource_type_group_count
};

enum : resource_type
{
	resource_type_file = 0xFFFFFFFD,
	resource_type_request = 0xFFFFFFFE,

	finish_request_type = 0xFFFFFFFF
};

struct file_cook_data
{
	enum { resource_type = resource_type_file };

	s8 file_path[1];

	uptr calculate_size( ) const;
};

struct request_cook_data
{
	enum { resource_type = resource_type_request };

	s8 file_path[1];

	uptr calculate_size( ) const;
};

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPES_H_INCLUDED