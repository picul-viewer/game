#include "request_builder.h"
#include <lib/memory.h>
#include <lib/strings.h>
#include <math/math_common.h>
#include "query_data.h"
#include "resource_type_registry.h"

namespace resource_system {

query_base::query_base( request_builder& in_builder, resource_type const in_resource_type, pointer const in_cook_data, uptr const in_cook_data_size ) :
	m_builder( in_builder ),
	m_resource_type( in_resource_type ),
	m_cook_data( in_cook_data ),
	m_cook_data_size( (u32)in_cook_data_size )
{
	ASSERT_CMP( in_cook_data, <, 4 * Gb );
}

void query_base::query( query_base& in_query )
{
	ASSERT_CMP( &m_builder, ==, &in_query.m_builder );

	in_query.add_dependent_query( *this );
	add_requested_query( in_query );
}

pointer query_base::cook_data( ) const
{
	return m_cook_data;
}

void query_base::add_dependent_query( query_base& in_query )
{
	dependent_query* const dq = m_builder.allocator( ).allocate( sizeof(dependent_query) );
	dq->m_query = &in_query;
	m_dependent_queries.insert( dq );
	++m_dependent_queries_count;
}

void query_base::add_requested_query( query_base& in_query )
{
	requested_query* const rq = m_builder.allocator( ).allocate( sizeof(requested_query) );
	rq->m_query = &in_query;
	m_requested_queries.insert( rq );
	++m_requested_queries_count;
}


void request_builder::create( pointer const in_memory, uptr const in_size )
{
	m_allocator.create( in_memory, in_size );

	m_queried_count = 0;
}

void request_builder::destroy( )
{
	m_allocator.destroy( );
}

linear_allocator& request_builder::allocator( )
{
	return m_allocator;
}

linear_allocator const& request_builder::allocator( ) const
{
	return m_allocator;
}

void request_builder::query_resource( query_base& in_query )
{
	ASSERT_CMP( &in_query.m_builder, ==, this );

	m_queried.insert( &in_query );
	++m_queried_count;
}

struct request_builder::request_dimensions
{
	uptr size_to_allocate;
	u32 finish_query_offset;
	u32 result_table_offset;
};

request_ptr&& request_builder::compile( ) const
{
	request_dimensions dimensions;
	calculate_dimensions( dimensions );

	pointer const memory = std_allocator( ).allocate( dimensions.size_to_allocate );

	linear_allocator allocator;
	allocator.create( memory, dimensions.size_to_allocate );

	compile( allocator, dimensions );

	return std::move( request_ptr( memory ) );
}

void request_builder::calculate_dimensions( request_dimensions& in_dimensions ) const
{
	uptr const alignment = 8;

	uptr size = 0;

	m_resources.for_each( [&size, alignment]( query_base* const in_query )
	{
		resource_type const type = in_query->m_resource_type;
		uptr const cook_data_size = in_query->m_cook_data_size;
		u32 const dependent_queries_count = in_query->m_dependent_queries_count;
		u32 const requested_queries_count = in_query->m_requested_queries_count;

		size += math::align_up( query_data::calculate_size( cook_data_size, dependent_queries_count, requested_queries_count ), alignment );
	} );

	size += sizeof(request_header);
	
	ASSERT_CMP( size, <, 4 * Gb );
	u32 const finish_query_offset = (u32)size;

	size += math::align_up( query_data::calculate_size( 0, 0, m_queried_count ), alignment );
	size += sizeof(query_callback_id);
	
	ASSERT_CMP( size, <, 4 * Gb );
	u32 const result_table_offset = (u32)size;

	size += m_resources_count * sizeof(query_result);

	in_dimensions.size_to_allocate = size;
	in_dimensions.finish_query_offset = finish_query_offset;
	in_dimensions.result_table_offset = result_table_offset;
}

void request_builder::compile( linear_allocator& in_allocator, request_dimensions const& in_dimensions ) const
{
	uptr const alignment = 8;

	// Allocate request header.
	ASSERT( sizeof(request_header) % alignment == 0 );
	request_header* const header = in_allocator.allocate( sizeof(request_header) );

	header->ref_count = 1;
	header->finish_query_offset = in_dimensions.finish_query_offset;

	u32 query_index = 0;

	query_result const* const result_table = (pointer)header + in_dimensions.result_table_offset;

	// Allocate all queries.
	m_resources.for_each( [&]( query_base* const in_query )
	{
		resource_type const type = in_query->m_resource_type;
		uptr const cook_data_size = in_query->m_cook_data_size;
		u32 const dependent_queries_count = in_query->m_dependent_queries_count;
		u32 const requested_queries_count = in_query->m_requested_queries_count;

		query_data* const query_data = in_allocator.allocate( math::align_up( query_data::calculate_size( cook_data_size, dependent_queries_count, requested_queries_count ), alignment ) );
		
		ptr const result_offset = (pointer)( result_table + query_index ) - (pointer)query_data;
		ASSERT_CMP( result_offset, <, 4 * Gb );
		ASSERT_CMP( result_offset, >=, 0 );

		query_data->waiting_factor = requested_queries_count;
		query_data->base_size = (u32)query_data::calculate_base_size( cook_data_size );
		query_data->index = (u32)result_offset;
		query_data->type = type;
		query_data->dependent_queries_count = dependent_queries_count;
		query_data->requested_queries_count = requested_queries_count;
		memory::copy( query_data->cook_data( ), in_query->cook_data( ), cook_data_size );

		in_query->m_result = query_data;

		++query_index;
	} );

	ASSERT( query_index == m_resources_count );

	// Resolve dependent queries links.
	m_resources.for_each( [&]( query_base* const in_query )
	{
		query_data* const data = in_query->m_result;

		u32 index = data->dependent_queries_count;

		in_query->m_dependent_queries.for_each( [data, &index]( query_base::dependent_query* const in_query_data )
		{
			--index;

			pointer const dependent_data = in_query_data->m_query->m_result;

			ptr const dependent_data_offset = dependent_data - (pointer)data;
			ASSERT_CMP( dependent_data_offset, <, 4 * Gb );
			ASSERT_CMP( dependent_data_offset, >=, 0 );

			data->dependent_queries( )[index] = (u32)dependent_data_offset;
		} );
	} );
	
	// Resolve requested queries links.
	m_resources.for_each( [&]( query_base* const in_query )
	{
		query_data* const data = in_query->m_result;

		u32 index = data->requested_queries_count;

		in_query->m_requested_queries.for_each( [data, &index]( query_base::requested_query* const in_query_data )
		{
			--index;

			pointer const requested_data = in_query_data->m_query->m_result;
			
			ptr const requested_data_offset = requested_data - (pointer)data;
			ASSERT_CMP( requested_data_offset, <, 4 * Gb );
			ASSERT_CMP( requested_data_offset, >=, 0 );

			data->requested_queries( )[index] = (u32)requested_data_offset;
		} );
	} );

	// Create identity query.
	resource_type const type = finish_request_type;
	uptr const cook_data_size = 0;
	u32 const dependent_queries_count = 0;
	u32 const requested_queries_count = m_queried_count;

	query_data* const data = in_allocator.allocate( math::align_up( query_data::calculate_size( cook_data_size, dependent_queries_count, requested_queries_count ), 8 ) );
	ASSERT( (pointer)data - (pointer)header == in_dimensions.finish_query_offset );

	data->base_size = (u32)query_data::calculate_base_size( cook_data_size );
	data->waiting_factor = requested_queries_count;
	// Store query data offset, using "index" member.
	data->index = in_dimensions.finish_query_offset;
	data->type = type;
	data->dependent_queries_count = dependent_queries_count;
	data->requested_queries_count = requested_queries_count;

	// Resolve requested queries links for identity query.
	u32 index = data->requested_queries_count;

	m_queried.for_each( [data, &index]( query_base* const in_query )
	{
		--index;

		pointer const requested_data = in_query->m_result;

		ptr const requested_data_offset = requested_data - (pointer)data;
		ASSERT_CMP( requested_data_offset, <, 4 * Gb );
		ASSERT_CMP( requested_data_offset, >=, 0 );

		data->requested_queries( )[index] = (u32)requested_data_offset;
	} );
}

} // namespace resource_system