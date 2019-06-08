#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED

#include <types.h>

namespace resource_system {

typedef pvoid query_result;

class queried_resources;

typedef void( *user_query_callback )( queried_resources&, pointer const, uptr const );
typedef void( *cook_functor )( pointer const, queried_resources& );

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED