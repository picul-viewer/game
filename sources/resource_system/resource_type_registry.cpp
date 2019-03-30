#include "resource_type_registry.h"
#include <macros.h>
#include "request_ptr.h"

namespace resource_system {

void resource_type_registry::create( )
{ }

resource_type_registry::resource_type_data& resource_type_registry::operator[]( resource_type const in_resource_type )
{
	u8 const type_group = in_resource_type >> 24u;
	u32 const type_index = in_resource_type & 0x00FFFFFF;

	ASSERT_CMP( type_group, <, (u8)resource_type_group_count );

	return m_groups[type_group][type_index];
}

resource_type_registry g_resource_type_registry;

} // namespace resource_system