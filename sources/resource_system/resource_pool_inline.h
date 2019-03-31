#ifndef GUARD_RESOURCE_POOL_INLINE_H_INCLUDED
#define GUARD_RESOURCE_POOL_INLINE_H_INCLUDED

#include <macros.h>
#include <math/math_common.h>
#include <intrin.h>

namespace resource_system {

template<typename Resource, uptr MaxSize>
void resource_pool<Resource, MaxSize>::create( pointer const in_memory )
{
	static_assert( MaxSize < 0x100000000, "too big size for resource pool" );

	pointer const memory = ( in_memory == nullptr ) ? virtual_allocator( ).allocate( memory_size ) : in_memory;

	m_table = memory;
	m_data = memory + table_size;
}

template<typename Resource, uptr MaxSize>
void resource_pool<Resource, MaxSize>::destroy( )
{
	virtual_allocator( ).deallocate( m_table );
}

template<typename Resource, uptr MaxSize>
bool resource_pool<Resource, MaxSize>::get( resource_instance_id const in_resource_instance_id, u32& out_index )
{
	u32 const hash = math::align_down( (u32)( _mm_crc32_u32( in_resource_instance_id, 0xFFFFFFFF ) % MaxSize ), 16u );

	__m128i value = _mm_setr_epi8( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 );
	__m128i const value_shift = _mm_set1_epi8( 16 );

	for ( u32 index = hash, i = 0; i < 15; index += 16, ++i )
	{
		__m128i const current_value = _mm_load_si128( (__m128i*)( m_table + index % MaxSize ) );

		__m128i const compare_result = _mm_cmpeq_epi8( current_value, value );

		if ( !_mm_testz_si128( compare_result, compare_result ) )
		{
			u32 mask = _mm_movemask_epi8( compare_result );

			do
			{
				unsigned long bit_index;
				_BitScanForward( &bit_index, mask );

				u32 const result_index = index % MaxSize + bit_index;

				if ( m_data[result_index] == in_resource_instance_id )
				{
					out_index = result_index;
					return true;
				}

				mask &= mask - 1;
			}
			while ( mask );
		}
		
		__m128i const compare_zero_result = _mm_cmpeq_epi8( current_value, _mm_setzero_si128( ) );
		
		if ( !_mm_testz_si128( compare_zero_result, compare_zero_result ) )
		{
			u32 const mask = _mm_movemask_epi8( compare_zero_result );

			unsigned long bit_index;
			_BitScanForward( &bit_index, mask );

			u32 const result_index = index % MaxSize + bit_index;

			m_table[result_index] = (u8)( result_index - hash + 1 );
			m_data[result_index] = in_resource_instance_id;
			out_index = result_index;

			__m128i const new_value = _mm_load_si128( (__m128i*)( m_table + index % MaxSize ) );

			return false;
		}

		value = _mm_add_epi8( value, value_shift );
	}

	FATAL_ERROR( "unable insert new resource into resource pool\n" );

	return false;
}


template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
void resource_pool1<Resource, TableSize, MaxSize, PageSize>::create( pointer const in_reserved_memory )
{
	static_assert( TableSize <= MaxSize, "invalid table for resource pool" );
	static_assert( TableSize >= 256, "invalid table for resource pool" );

	static_assert( MaxSize < 0x100000000, "invalid max size for resource pool" );

	static_assert( PageSize % Memory_Page_Size == 0, "invalid page size for resource pool" );

	pointer const memory = ( in_reserved_memory == nullptr ) ? virtual_allocator( ).reserve( nullptr, memory_size ) : in_reserved_memory;

	m_table = memory;
	m_data = memory + table_size;

	virtual_allocator( ).commit( m_table, math::align_up( table_record_size * TableSize, PageSize ) );
	virtual_allocator( ).commit( m_data, math::align_up( sizeof(Resource) * TableSize, PageSize ) );

	m_push_index = 0;
	m_last_index = TableSize;
}

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
void resource_pool1<Resource, TableSize, MaxSize, PageSize>::destroy( )
{
	virtual_allocator( ).release( m_table );
}

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
bool resource_pool1<Resource, TableSize, MaxSize, PageSize>::get( resource_instance_id const in_resource_instance_id, u32& out_index )
{
	struct table_record
	{
		u32 id;
		u32 next;
	};

	u32 const hash = _mm_crc32_u32( in_resource_instance_id, 0xFFFFFFFF );

	u32 const comparand = in_resource_instance_id + 1;
	ASSERT( comparand != 0 );

	u32 const base_index = hash % TableSize;

	table_record* const base_record = (table_record*)m_table + base_index;

	if ( base_record->id == comparand )
	{
		out_index = base_index;
		return true;
	}

	bool const insert_front = base_record->id == 0;

	table_record* current_record = base_record;

	while ( ( current_record->next != 0 ) && ( current_record->id != comparand ) )
	{
		table_record* next_record = (table_record*)m_table + current_record->next;

		if ( next_record->id == 0 )
		{
			next_record->id = m_push_index;
			m_push_index = current_record->next;

			current_record->next = next_record->next;
		}
		else
			current_record = next_record;
	}

	if ( current_record->id == comparand )
	{
		out_index = (u32)( current_record - (table_record*)m_table );
		return true;
	}

	if ( insert_front )
	{
		base_record->id = comparand;
		out_index = base_index;
		return false;
	}

	if ( m_push_index == 0 )
	{
		ASSERT_CMP( m_last_index, <, MaxSize );

		current_record->next = m_last_index;
		out_index = m_last_index;
		table_record* const new_record = (table_record*)m_table + m_last_index;
		
		++m_last_index;

		pointer const table_end = (table_record*)m_table + m_last_index;
		if ( (uptr)table_end % PageSize <= sizeof(table_record) )
			virtual_allocator( ).commit( table_end.align_down( PageSize ), PageSize );

		pointer const data_end = (Resource*)m_data + m_last_index;
		if ( (uptr)data_end % PageSize <= sizeof(Resource) )
			virtual_allocator( ).commit( data_end.align_down( PageSize ), PageSize );

		new_record->id = comparand;
		new_record->next = 0;
	}
	else
	{
		current_record->next = m_push_index;
		out_index = m_push_index;
		table_record* const new_record = (table_record*)m_table + m_push_index;

		m_push_index = new_record->id;

		new_record->id = comparand;
		new_record->next = 0;
	}

	return false;
}



template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
void resource_pool2<Resource, TableSize, MaxSize, PageSize>::create( pointer const in_reserved_memory )
{
	static_assert( TableSize * 4 <= MaxSize, "invalid table for resource pool" );
	static_assert( TableSize >= 16, "invalid table for resource pool" );
	static_assert( ( TableSize & ( TableSize - 1 ) ) == 0, "invalid table for resource pool" );

	static_assert( MaxSize <= 0x00040000, "invalid max size for resource pool" );
	static_assert( MaxSize % 4 == 0, "invalid max size for resource pool" );

	static_assert( PageSize % Memory_Page_Size == 0, "invalid page size for resource pool" );

	pointer const memory = ( in_reserved_memory == nullptr ) ? virtual_allocator( ).reserve( nullptr, memory_size ) : in_reserved_memory;

	m_table = memory;
	m_data = memory + table_size;

	virtual_allocator( ).commit( m_table, math::align_up( sizeof(u32) * 4 * TableSize, PageSize ) );
	virtual_allocator( ).commit( m_data, math::align_up( sizeof(Resource) * 4 * TableSize, PageSize ) );

	m_push_index = 0;
	m_last_index = TableSize;
}

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
void resource_pool2<Resource, TableSize, MaxSize, PageSize>::destroy( )
{
	virtual_allocator( ).release( m_table );
}

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
bool resource_pool2<Resource, TableSize, MaxSize, PageSize>::get( resource_instance_id const in_resource_instance_id, u32& out_index )
{
	ASSERT( in_resource_instance_id != 0 );
	ASSERT( in_resource_instance_id <0x10000000 );

	u32 const transformed_id =
		( ( in_resource_instance_id << 0 ) & 0x0000007F ) |
		( ( in_resource_instance_id << 1 ) & 0x00007F00 ) |
		( ( in_resource_instance_id << 2 ) & 0x007F0000 ) |
		( ( in_resource_instance_id << 3 ) & 0x7F000000 );

	__m128i const comparand = _mm_set1_epi32( transformed_id );
	__m128i const hashes_mask = _mm_set1_epi8( (u8)0x7F );

	u32 const hash = _mm_crc32_u32( in_resource_instance_id, 0xFFFFFFFF );

	u32* current_record;
	__m128i current_value;
	u32 free_index = 0;

	u32 table_index = hash % TableSize;

	do
	{
		current_record = m_table + table_index * 4;
		current_value = _mm_load_si128( (__m128i*)current_record );

		__m128i const hashes = _mm_and_si128( current_value, hashes_mask );
		__m128i const compare_result = _mm_cmpeq_epi32( hashes, comparand );

		if ( !_mm_testz_si128( compare_result, compare_result ) )
		{
			u32 const mask = _mm_movemask_ps( _mm_castsi128_ps( compare_result ) );

			unsigned long bit_index;
			_BitScanForward( &bit_index, mask );

			u32 const resource_index = table_index * 4 + bit_index;

			mt_u32& ref_counter = m_data[resource_index].ref_counter( );

			u32 ref_count;
			SPIN_LOCK( ( ref_count = interlocked_exchange( ref_counter, 0xFFFFFFFF ) ) == 0xFFFFFFFF );

			if ( ref_count != 0 )
			{
				ref_counter = ref_count + 1;
				out_index = table_index * 4 + bit_index;
				return true;
			}
		}

		if ( free_index == 0 )
		{
			__m128i const compare_zero_result = _mm_cmpeq_epi32( hashes, _mm_setzero_si128( ) );

			if ( !_mm_testz_si128( compare_zero_result, compare_zero_result ) )
			{
				u32 const mask = _mm_movemask_ps( _mm_castsi128_ps( compare_zero_result ) );
				
				unsigned long bit_index;
				_BitScanForward( &bit_index, mask );

				free_index = table_index * 4 + bit_index + 1;
			}
		}

		table_index = _mm_movemask_epi8( current_value );
	}
	while ( table_index != 0 );

	if ( free_index )
	{
		--free_index;
		
		m_table[free_index] |= transformed_id;

		out_index = free_index;
		return false;
	}

	if ( m_push_index == 0 )
		free_index = m_last_index;
	else
	{
		free_index = m_push_index;
		m_push_index = m_table[free_index];
	}

	ASSERT_CMP( free_index, <, 0x10000 );

	out_index = 4 * free_index;

	__m128i const vec_index_half_bytes =
		_mm_and_si128(
			_mm_setr_epi32( free_index, free_index / 16, free_index / 256, free_index / 4096 ),
			_mm_set1_epi32( 0xF )
		);

	__m128i const free_index_mask =
		_mm_and_si128(
			_mm_or_si128(
				_mm_or_si128( _mm_slli_epi32( vec_index_half_bytes, 7 ), _mm_slli_epi32( vec_index_half_bytes, 14 ) ),
				_mm_or_si128( _mm_slli_epi32( vec_index_half_bytes, 21 ), _mm_slli_epi32( vec_index_half_bytes, 28 ) )
			),
			_mm_set1_epi8( (u8)0x80 )
		);

	_mm_store_si128( (__m128i*)current_record, _mm_or_si128( free_index_mask, current_value ) );

	if ( m_push_index == 0 )
	{
		ASSERT_CMP( m_last_index, <, MaxSize / 4 );

		++m_last_index;

		pointer const table_end = m_table + m_last_index * 4;
		if ( (uptr)table_end % PageSize <= sizeof(__m128i) )
			virtual_allocator( ).commit( table_end.align_down( PageSize ), PageSize );

		pointer const data_end = m_data + m_last_index * 4;
		if ( (uptr)data_end % PageSize <= sizeof(Resource) )
			virtual_allocator( ).commit( data_end.align_down( PageSize ), PageSize );
	}

	__m128i const new_value = _mm_setr_epi32( transformed_id, 0, 0, 0 );
	
	_mm_store_si128( (__m128i*)( m_table + free_index * 4 ), new_value );

	return false;
}

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
u32 resource_pool2<Resource, TableSize, MaxSize, PageSize>::add_ref( u32 const in_resource_id )
{
	mt_u32& ref_counter = m_data[in_resource_id].ref_counter( );

	u32 ref_count;
	SPIN_LOCK( ( ref_count = interlocked_exchange( ref_counter, 0xFFFFFFFF ) ) == 0xFFFFFFFF );

	ASSERT( ref_count != 0 );

	++ref_count;

	ref_counter = ref_count;
	return ref_count;
}

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
u32 resource_pool2<Resource, TableSize, MaxSize, PageSize>::release( u32 const in_resource_id )
{
	mt_u32& ref_counter = m_data[in_resource_id].ref_counter( );

	u32 ref_count;
	SPIN_LOCK( ( ref_count = interlocked_exchange( ref_counter, 0xFFFFFFFF ) ) == 0xFFFFFFFF );

	ASSERT( ref_count != 0 );

	--ref_count;

	ref_counter = ref_count;
	return ref_count;
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_POOL_INLINE_H_INCLUDED