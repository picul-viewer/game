#ifndef GUARD_CORE_MPSC_QUEUE_H_INLCUDED
#define GUARD_CORE_MPSC_QUEUE_H_INLCUDED

#include <types.h>
#include "interlocked.h"

namespace sys {

// Multiple producers, single consumer.
template<typename T>
class mpsc_queue
{
public:
	typedef T value_type;

public:
	void create( pointer const memory, uptr const size );
	void destroy( );

	bool push( value_type const& value );
	bool push( value_type const* const values, u32 const count );
	bool pop( value_type& value );
	
	pointer data( ) const;

private:
	union
	{
		mt_u32			m_pre_push_index;
		u8				m_padding0[Cache_Line];
	};
	union
	{
		mt_u32			m_push_index;
		u8				m_padding1[Cache_Line];
	};
	union
	{
		u32				m_pop_index;
		u8				m_padding2[Cache_Line];
	};
	union
	{
		mt_u32			m_current_size;
		u8				m_padding3[Cache_Line];
	};
	value_type*			m_data;
	u32					m_index_mask;
};

} // namespace sys

#include "mpsc_queue_inline.h"

#endif // #ifndef GUARD_CORE_MPSC_QUEUE_H_INLCUDED