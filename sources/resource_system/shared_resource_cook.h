#ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_COOK_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_COOK_H_INCLUDED

#include <types.h>
#include <system/interlocked.h>
#include "default_resource_cook.h"
#include "shared_resource.h"

namespace resource_system {

template<typename Resource, typename ResourceCook>
class shared_resource_cook : public default_resource_cook<Resource, ResourceCook>
{
protected:
	void init( shared_resource_id const in_id );

	void check_resource_exists(
		void ( ResourceCook::*in_callback )( Resource* const )
	);

	void finish( Resource* const in_result );

private:
	template<typename ContainerResource, typename ContainerResourceHandle, u32 ContainerTableSize>
	friend class shared_resource_container;

	typedef shared_resource_cook<Resource, ResourceCook> this_type;

	shared_resource_id id( ) const;

	void add_subscriber( this_type* const in_cook );

private:
	using resource_cook::finish_cook;

	void finish_impl( pvoid const in_result );

private:
	this_type* m_next_subscriber;
	shared_resource_id m_id;
	u32 m_initial_ref_count;
	u32 m_parent_task_offset;
	u32 m_result_index;

};

} // namespace resource_system

using resource_system::shared_resource_cook;

#include "shared_resource_cook_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_COOK_H_INCLUDED