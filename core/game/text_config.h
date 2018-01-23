#ifndef __core_text_config_h_included_
#define __core_text_config_h_included_

#include <core/types.h>
#include <core/std.h>

class text_config
{
public:
	text_config( pointer data );
	void set( pointer data );

	template<typename T>
	T read( );
	
	void read_str( pstr data );

protected:
	pointer	m_begin;
	pointer	m_pointer;
};

template<typename T>
inline T text_config::read( )
{
	ASSERT( false, "unsupported type for text config!" );
}

#define READ_PROC( type, mask ) \
template<> \
inline type text_config::read( ) \
{ \
	type data; \
	int pos; \
	sscanf( m_pointer, mask, &data, &pos ); \
	m_pointer += pos; \
	return data; \
}

READ_PROC( s8,	"%hhi%n" )
READ_PROC( s16,	"%hi%n" )
READ_PROC( s32,	"%li%n" )
READ_PROC( s64,	"%lli%n" )

READ_PROC( u8,	"%hhu%n" )
READ_PROC( u16,	"%hu%n" )
READ_PROC( u32,	"%lu%n" )
READ_PROC( u64,	"%llu%n" )

READ_PROC( float,	"%f%n" )
READ_PROC( double,	"%lf%n" )

#undef READ_PROC

#endif // #ifndef __core_text_config_h_included_