#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED

#include <types.h>

namespace resource_system {

typedef u32 resource_type;
typedef u32 resource_instance_id;
typedef u64 resource_id;

typedef u64 query_result;

class queried_resources;

typedef u32 query_callback_id;
typedef void( *query_callback )( queried_resources&, pointer const );

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED