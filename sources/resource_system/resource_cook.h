#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_DATA_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_DATA_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"
#include <utils/resource_types.h>

namespace resource_system {

class resource_cook
{
public:
	resource_cook( resource_type const type );

	inline resource_type type( ) const { return m_type; }
	inline u32& size( ) { return m_size; }

protected:
	template<typename T, void(T::*Callback)( queried_resources& )>
	static void query_callback( pointer const in_this_ptr, queried_resources& resources );

	void query_resources(
		resource_cook* const* const in_cooks,
		u32 const in_cook_count,
		cook_functor const& in_callback,
		u32 const in_functor_thread_index
	);

	void finish( query_result const result );

private:
	friend class resource_system;
	inline u32& parent_query( ) { return m_parent_query_data_offset; }
	inline u32& result_index( ) { return m_result_index; }

private:
	u32 m_size;
	resource_type m_type;
	u32 m_parent_query_data_offset;
	u32 m_result_index;

};

template<typename T, void(T::*Callback)( queried_resources& )>
void resource_cook::query_callback( pointer const in_this_ptr, queried_resources& resources )
{
	( ( (T*)in_this_ptr )->*Callback )( resources );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_DATA_H_INCLUDED