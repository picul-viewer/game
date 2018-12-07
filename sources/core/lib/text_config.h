#ifndef __core_text_config_h_included_
#define __core_text_config_h_included_

#include <types.h>
#include "config.h"
#include "weak_string.h"
#include "fixed_string.h"

class text_config
{
public:
	text_config( ) = default;

	text_config( config const& cfg );
	text_config( pointer const data, uptr const size );
	void create( config const& cfg );
	void create( pointer const data, uptr const size );

	template<typename T>
	T read( );
	void read_str( pstr const data );
	template<typename Arg>
	void read_mask( pcstr const mask, Arg* const arg );
	void skip_line( );
	
	template<typename T>
	void write( T const& value );
	void write_str( pstr const data );
	template<typename ... Args>
	void write_mask( pcstr const mask, Args const& ... args );
	
	pointer get_pointer( ) const;

	bool is_valid( ) const;

protected:
	pointer	m_pointer;
	pointer m_end;
};

template<typename T>
inline T text_config::read( )
{
	static_assert( false, "unsupported type for text config!" );
}

#define READ_PROC( type, mask ) \
template<> \
inline type text_config::read( ) \
{ \
	type data; \
	u32 pos; \
	_snscanf( m_pointer, m_end - m_pointer, mask, &data, &pos ); \
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

#define TEXT_CONFIG_READ_MASK( mask ) mask "%n"

template<typename Arg>
void text_config::read_mask( pcstr const mask, Arg* const arg )
{
	u32 pos;
	_snscanf( m_pointer, m_end - m_pointer, mask, arg, &pos );
	m_pointer += pos;
}

template<typename T>
inline void text_config::write( T const& value )
{
	static_assert( false, "unsupported type for text config!" );
}

#define WRITE_PROC( type, mask ) \
template<> \
inline void text_config::write( type const& value ) \
{ \
	m_pointer += snprintf( m_pointer, m_end - m_pointer, mask, value ); \
}

WRITE_PROC( s8,		"%hhi" )
WRITE_PROC( s16,	"%hi" )
WRITE_PROC( s32,	"%li" )
WRITE_PROC( s64,	"%lli" )

WRITE_PROC( u8,		"%hhu" )
WRITE_PROC( u16,	"%hu" )
WRITE_PROC( u32,	"%lu" )
WRITE_PROC( u64,	"%llu" )

WRITE_PROC( float,	"%f" )
WRITE_PROC( double,	"%lf" )

#undef WRITE_PROC

#define TEXT_CONFIG_WRITE_MASK( mask ) mask

template<typename ... Args>
void text_config::write_mask( pcstr const mask, Args const& ... args )
{
	m_pointer += snprintf( m_pointer, m_end - m_pointer, mask, args ... );
}

#endif // #ifndef __core_text_config_h_included_