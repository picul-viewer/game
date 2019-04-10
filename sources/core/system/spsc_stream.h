#ifndef GUARD_CORE_SPSC_STREAM_H_INCLUDED
#define GUARD_CORE_SPSC_STREAM_H_INCLUDED

#include <types.h>
#include "interlocked.h"

namespace sys {

class spsc_stream
{
public:
	spsc_stream( ) = default;

	spsc_stream( pointer const memory, uptr const size );
	void create( pointer const memory, uptr const size );

	void clear( );

	pvoid data( ) const;

	template<typename T>
	inline T const& read( );
	
	pvoid read_data( u32 const size );
	
	template<typename T>
	inline void write( T const& value );
	
	void write_data( pcvoid const data, u32 const size );
	
protected:
	union
	{
		mt_u32 m_push_pointer;
		u8 padding_0[Cache_Line];
	};
	union
	{
		u32 m_pop_pointer;
		u8 padding_1[Cache_Line];
	};
	pointer	m_data;
	u32 m_size;
};


template<typename T>
T const& spsc_stream::read( )
{
	T* const data	= (T*)read_data( sizeof(T) );
	return			*data;
}

template<typename T>
void spsc_stream::write( T const& value )
{
	write_data		( &value, sizeof(T) );
}

} // namespace sys

#endif // #ifndef GUARD_CORE_SPSC_STREAM_H_INCLUDED