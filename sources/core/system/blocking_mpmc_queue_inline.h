#ifndef GUARD_CORE_BLOCKING_MPMC_QUEUE_INLINE_H_INLCUDED
#define GUARD_CORE_BLOCKING_MPMC_QUEUE_INLINE_H_INLCUDED

#include <macros.h>

template<typename T>
void sys::blocking_mpmc_queue<T>::create( pointer const memory, uptr const size )
{
	ASSERT_CMP					( size, <=, 0x100000000 );
	ASSERT						( ( size & ( size - 1 ) ) == 0 );

	m_data						= memory;
	m_index_mask				= (u32)( size - 1 );

	m_pre_push_index			= 0;
	m_push_index				= 0;
	m_pre_pop_index				= 0;
	m_pop_index					= 0;

	m_current_size				= 0;

	m_empty_event.create		( false, false );
}

template<typename T>
void sys::blocking_mpmc_queue<T>::destroy( )
{
	m_empty_event.destroy		( );
}

template<typename T>
void sys::blocking_mpmc_queue<T>::push( value_type const& value )
{
	u32 const new_push_index	= interlocked_inc( m_pre_push_index );
	u32 const target_index		= new_push_index - 1;
	
	m_data[target_index & m_index_mask] = value;
	
	SPIN_LOCK					( m_push_index != target_index );
	
	m_push_index				= new_push_index;
	
	store_fence					( );

	if ( interlocked_inc( m_current_size ) <= 0 )
	{
		m_empty_event.set		( );
	}
}

template<typename T>
void sys::blocking_mpmc_queue<T>::push( value_type const* const values, u32 const count )
{
	u32 const new_push_index	= interlocked_add( m_pre_push_index, count );
	u32 const target_index		= new_push_index - count;

	for ( u32 i = target_index, j = 0; i < new_push_index; ++i, ++j )
		m_data[i & m_index_mask] = values[j];

	SPIN_LOCK					( m_push_index != target_index );
	
	m_push_index				= new_push_index;

	store_fence					( );

	if ( interlocked_add( m_current_size, count ) < count )
	{
		m_empty_event.set		( );
	}
}

template<typename T>
void sys::blocking_mpmc_queue<T>::pop( value_type& value )
{
	while ( interlocked_dec( m_current_size ) < 0 )
	{
		m_empty_event.wait		( );
		interlocked_inc			( m_current_size );
	}
	
	u32 const new_pop_index		= interlocked_inc( m_pre_pop_index );
	u32 const target_index		= new_pop_index - 1;
	
	value						= m_data[target_index & m_index_mask];
	
	SPIN_LOCK					( m_pop_index != target_index );
	
	m_pop_index					= new_pop_index;
}

template<typename T>
pointer sys::blocking_mpmc_queue<T>::data( ) const
{
	return m_data;
}

#endif // #ifndef GUARD_CORE_BLOCKING_MPMC_QUEUE_INLINE_H_INLCUDED