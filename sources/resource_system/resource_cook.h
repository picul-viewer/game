#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED

#include <types.h>
#include <macros.h>
#include <lib/allocator.h>
#include "resource_creator.h"
#include "resource_system_types.h"
#include "resource_system_utils.h"

namespace resource_system {

class resource_cook : public resource_creator
{
protected:
	typedef void ( *cook_destroyer )( pointer );

protected:
	void init( cook_destroyer const in_destroyer );

	void finish_cook( pvoid const in_result );
	void finish_cook( pvoid const in_result, u32 const in_task_offset, u32 const in_result_index );

	void get_result_links( u32& in_parent_data_offset, u32& in_result_index ) const;

private:
	cook_destroyer m_destroyer;

};

} // namespace resource_system

using resource_system::resource_cook;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED