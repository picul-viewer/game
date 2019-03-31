#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"
#include "queried_resources.h"

#include "resource_types.h"
//#include <render/resource_types.h>
//#include <physics/resource_types.h>

namespace resource_system {

class resource_type_registry
{
public:
	void create( );

	template<typename ResourceType>
	inline void register_resource_type( resource_type const in_resource_type );
	
private:
	friend class resource_system;
	friend class request_builder;

	// Functor for creating the coresponding resource.
	typedef query_result( *resource_creator )( pointer const, queried_resources const& );

	typedef bool( *resource_manager )( pointer const, resource_instance_id& );

	struct resource_type_data
	{
		resource_creator m_creator;
		resource_manager m_manager;
	};

	resource_type_data& operator[]( resource_type const in_resource_type );

private:
	resource_type_data* m_groups[resource_type_group_count];

	//resource_type_data m_render_resource_types[render::resource_type_count & 0x00FFFFFF];
	//resource_type_data m_physics_resource_types[physics::resource_type_count & 0x00FFFFFF];
};

extern resource_type_registry g_resource_type_registry;

} // namespace resource_system

#include "resource_type_registry_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_H_INCLUDED