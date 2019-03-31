#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_INLINE_H_INCLUDED

namespace resource_system {

template<typename ResourceType>
void resource_type_registry::register_resource_type( resource_type const in_resource_type )
{
	resource_type_data& data = operator[]( in_resource_type );

	data.m_creator = (resource_creator)&ResourceType::create_resource;
	data.m_manager = (resource_manager)&ResourceType::manage_resource;
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_INLINE_H_INCLUDED