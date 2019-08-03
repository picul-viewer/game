#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED

#include <types.h>
#include <macros.h>
#include <lib/allocator.h>
#include "resource_system_types.h"

namespace resource_system {

class resource_cook
{
protected:
	void init( query_functor const in_destroyer );

	void create_child_resources(
		query_info const* const in_queries,
		u32 const in_query_count,
		cook_task_info const& in_callback_info
	);

	template<typename ... Cooks>
	void create_child_resources(
		cook_task_info const& in_callback_info,
		Cooks* ... in_cooks
	);

	void execute_tasks(
		cook_task_info const* const in_tasks,
		u32 const in_task_count,
		cook_task_info const& in_callback
	);

	void finish( pvoid const in_result );

private:
	friend class resource_system;
	inline u32& parent_query( ) { return m_parent_query_data_offset; }
	inline u32& result_index( ) { return m_result_index; }

private:
	query_functor m_destroyer;
	u32 m_parent_query_data_offset;
	u32 m_result_index;

};

template<typename ... Cooks>
void fill_query_info( query_info* const in_info, Cooks* ... in_cooks );

template<typename Cook, typename ... Cooks>
void fill_query_info( query_info* const in_info, Cook* in_cook, Cooks const& ... in_cooks )
{
	ASSERT( in_cook != nullptr );

	in_cook->fill_query( in_info[0] );
	fill_query_info( in_info + 1, in_cooks ... );
}

template<>
void fill_query_info( query_info* const in_info )
{ }

template<typename ... Cooks>
void resource_cook::create_child_resources( cook_task_info const& in_callback_info, Cooks* ... in_cooks )
{
	u32 const count = type_count<Cooks ...>::value;
	query_info* const queries = stack_allocate( count * sizeof(query_info) );

	fill_query_info( queries, in_cooks ... );

	create_child_resources( queries, count, in_callback_info );
}

} // namespace resource_system

using resource_system::resource_cook;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED