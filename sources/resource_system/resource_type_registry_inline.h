#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_INLINE_H_INCLUDED

namespace resource_system {

namespace __resource_system_resource_type_registry_inline_detail {

template<typename T, void(T::*Executor)( )>
static void executor_wrapper( pointer const in_this_ptr, queried_resources& )
{
	( ( (T*)in_this_ptr )->*Executor )( );
}

} // namespace __resource_system_resource_type_registry_inline_detail

template<typename ResourceType>
void resource_type_registry::register_resource_type( )
{
	using namespace __resource_system_resource_type_registry_inline_detail;

	typedef typename ResourceType::cook_type ResourceCookType;

	resource_type_data& data = operator[]( (resource_type)ResourceCookType::cook_resource_type );

	data.executor = (cook_functor)&executor_wrapper<ResourceCookType, &ResourceCookType::create_resource>;
	data.executor_thread_index = (u32)ResourceCookType::create_resource_thread_index;
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_INLINE_H_INCLUDED