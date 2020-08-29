#include "resource_cook.h"
#include "resource_system.h"

namespace resource_system {

resource_cook::resource_cook( cook_destroyer const in_destroyer ) :
	m_destroyer( in_destroyer )
{ }

void resource_cook::finish_cook( pvoid const in_result )
{
	g_resource_system.set_current_task_result( in_result );

	if ( this )
		m_destroyer( this );
}

void resource_cook::finish_cook( pvoid const in_result, u32 const in_task_offset, u32 const in_result_index )
{
	g_resource_system.set_task_result( in_task_offset, in_result_index, in_result );

	m_destroyer( this );
}

void resource_cook::get_result_links( u32& in_parent_data_offset, u32& in_result_index ) const
{
	g_resource_system.get_current_result_data( in_parent_data_offset, in_result_index );
}

} // namespace resource_system