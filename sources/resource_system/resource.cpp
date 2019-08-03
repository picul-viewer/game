#include "resource.h"
#include "resource_system.h"

namespace resource_system {

void resource::delete_resource( query_info const& in_info )
{
	g_resource_system.destroy_resource( in_info );
}

} // namespace resource_system