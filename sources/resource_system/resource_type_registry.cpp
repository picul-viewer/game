#include "resource_type_registry.h"
#include <macros.h>

namespace resource_system {

resource_type_registry::resource_type_data& resource_type_registry::operator[]( resource_type const in_resource_type )
{
	ASSERT_CMP( in_resource_type, <, resource_type_count );

	return m_data[in_resource_type];
}

resource_type_registry g_resource_type_registry;

} // namespace resource_system