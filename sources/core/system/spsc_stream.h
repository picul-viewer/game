#ifndef __core_spsc_stream_h_included_
#define __core_spsc_stream_h_included_

#include <types.h>
#include "interlocked.h"

mem_align(16)
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

#endif // #ifndef __core_spsc_stream_h_included_