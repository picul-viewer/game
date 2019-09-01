#ifndef GUARD_CORE_MPSC_QUEUE_INLINE_H_INLCUDED
#define GUARD_CORE_MPSC_QUEUE_INLINE_H_INLCUDED

#include <macros.h>

template<typename T>
void sys::mpsc_queue<T>::create( pointer const memory, uptr const size )
{
	ASSERT_CMP					( size, <=, 0x100000000 );
	ASSERT						( ( size & ( size - 1 ) ) == 0 );

	m_data						= memory;
	m_index_mask				= (u32)( size - 1 );

	m_pre_push_index			= 0;
	m_push_index				= 0;
	m_pop_index					= 0;

	m_current_size				= 0;
}

template<typename T>
void sys::mpsc_queue<T>::destroy( )
{ }

template<typename T>
bool sys::mpsc_queue<T>::push( value_type const& value )
{
	u32 const new_push_index	= interlocked_inc( m_pre_push_index );
	u32 const target_index		= new_push_index - 1;
	
	m_data[target_index & m_index_mask] = value;
	
	SPIN_LOCK					( m_push_index != target_index );
	
	m_push_index				= new_push_index;
	
	store_fence					( );

	u32 const current_size		= interlocked_inc( m_current_size );
	return current_size <= 1;
}

template<typename T>
bool sys::mpsc_queue<T>::push( value_type const* const values, u32 const count )
{
	u32 const new_push_index	= interlocked_add( m_pre_push_index, count );
	u32 const target_index		= new_push_index - count;

	for ( u32 i = target_index, j = 0; i < new_push_index; ++i, ++j )
		m_data[i & m_index_mask] = values[j];

	SPIN_LOCK					( m_push_index != target_index );
	
	m_push_index				= new_push_index;

	store_fence					( );

	u32 const current_size		= interlocked_add( m_current_size, count );
	return current_size <= count;
}

template<typename T>
bool sys::mpsc_queue<T>::pop( value_type& value )
{
	if ( interlocked_exchange_add( m_current_size, (u32)-1 ) == 0 )
	{
		interlocked_inc			( m_current_size );
		return false;
	}
	
	u32 const pop_index			= m_pop_index;
	u32 const new_pop_index		= pop_index + 1;
	
	value						= m_data[pop_index & m_index_mask];

	m_pop_index					= new_pop_index;

	return true;
}

template<typename T>
pointer sys::mpsc_queue<T>::data( ) const
{
	return m_data;
}

template<typename T>
bool sys::mpsc_queue<T>::empty( ) const
{
	return m_push_index == m_pop_index;
}

#endif // #ifndef GUARD_CORE_MPSC_QUEUE_INLINE_H_INLCUDED