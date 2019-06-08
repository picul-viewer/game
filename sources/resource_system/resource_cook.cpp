#include "resource_cook.h"
#include "resource_system.h"

namespace resource_system {

resource_cook::resource_cook( resource_type const type ) :
	m_type( type )
{ }

void resource_cook::query_resources(
	resource_cook* const* const in_cooks,
	u32 const in_cook_count,
	cook_functor const& in_callback,
	u32 const in_functor_thread_index
)
{
	g_resource_system.query_child_resources( in_cooks, in_cook_count, this, in_callback, in_functor_thread_index );
}

void resource_cook::finish( query_result const result )
{
	g_resource_system.finish_cook( this, result );
}

} // namespace resource_system