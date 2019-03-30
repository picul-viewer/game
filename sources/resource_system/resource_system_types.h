#ifndef __resource_system_resource_system_types_h_included_
#define __resource_system_resource_system_types_h_included_

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

#endif // #ifndef __resource_system_resource_system_types_h_included_