#ifndef __resource_system_resource_type_registry_inline_h_included_
#define __resource_system_resource_type_registry_inline_h_included_

namespace resource_system {

template<typename ResourceType>
void resource_type_registry::register_resource_type( resource_type const in_resource_type )
{
	resource_type_data& data = operator[]( in_resource_type );

	data.m_creator = (resource_creator)&ResourceType::create_resource;
	data.m_manager = (resource_manager)&ResourceType::manage_resource;
}

} // namespace resource_system

#endif // #ifndef __resource_system_resource_type_registry_inline_h_included_