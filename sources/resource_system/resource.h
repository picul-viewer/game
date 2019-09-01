#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"

namespace resource_system {

class resource
{
protected:
	void destroy( task_info const& in_info );
	void destroy_sync( task_info const& in_info );

};

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_H_INCLUDED