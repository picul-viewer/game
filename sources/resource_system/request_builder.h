#ifndef GUARD_RESOURCE_SYSTEM_REQUEST_BUILDER_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_REQUEST_BUILDER_H_INCLUDED

#include <types.h>
#include <lib/linear_allocator.h>
#include <lib/embedded_list.h>
#include "resource_system_types.h"
#include "request_ptr.h"

namespace resource_system {

class request_builder;

class query_base
{
public:
	void query( query_base& in_query );

	pointer cook_data( ) const;

protected:
	query_base( request_builder& in_builder, resource_type const in_resource_type, pointer const in_cook_data, uptr const in_cook_data_size );

private:
	query_base( ) = delete;
	query_base( query_base const& ) = delete;
	query_base& operator=( query_base const& ) = delete;
	query_base( query_base&& ) = delete;
	query_base&& operator=( query_base&& ) = delete;
	
private:
	friend class request_builder;

	struct dependent_query
	{
		dependent_query* m_next;
		query_base* m_query;
	};

	struct requested_query
	{
		requested_query* m_next;
		query_base* m_query;
	};
	
	void add_dependent_query( query_base& in_query );
	void add_requested_query( query_base& in_query );
	
private:
	friend class request_builder;
	query_base* m_next;
	query_base* m_next_requested;

private:
	pointer m_result;
	request_builder& m_builder;
	embedded_list<dependent_query, &dependent_query::m_next> m_dependent_queries;
	embedded_list<requested_query, &requested_query::m_next> m_requested_queries;
	pointer m_cook_data;
	u32 m_cook_data_size;
	resource_type m_resource_type;
	u32 m_dependent_queries_count;
	u32 m_requested_queries_count;

};

template<typename CookDataType>
class query : public query_base
{
public:
	CookDataType* operator->( );
	CookDataType const* operator->( ) const;
	
private:
	friend class request_builder;
	query( request_builder& in_builder, CookDataType* const in_cook_data );

	query( ) = delete;
	query( query const& ) = delete;
	query& operator=( query const& ) = delete;
	query( query&& ) = delete;
	query&& operator=( query&& ) = delete;

};

class request_builder
{
public:
	void create( pointer const in_memory, uptr const in_size );
	void destroy( );

	linear_allocator& allocator( );
	linear_allocator const& allocator( ) const;

	template<typename CookDataType>
	query<CookDataType>& create_query( CookDataType* const in_cook_data );

	void query_resource( query_base& in_query );
	
	request_ptr&& compile( ) const;

private:
	struct request_dimensions;
	void calculate_dimensions( request_dimensions& in_dimensions ) const;
	void compile( linear_allocator& in_allocator, request_dimensions const& in_dimensions ) const;

private:
	linear_allocator m_allocator;
	embedded_list<query_base, &query_base::m_next> m_resources;
	embedded_list<query_base, &query_base::m_next_requested> m_queried;
	u32 m_resources_count;
	u32 m_queried_count;

};

} // namespace resource_system

#include "request_builder_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_REQUEST_BUILDER_H_INCLUDED