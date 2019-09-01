#include "resource.h"
#include "resource_system.h"

namespace resource_system {

void resource::destroy( task_info const& in_info )
{
	g_resource_system.create_tasks( nullptr, &in_info, 1, false );
}

void resource::destroy_sync( task_info const& in_info )
{
	g_resource_system.create_subtasks( nullptr, &in_info, 1, false );
}

} // namespace resource_system