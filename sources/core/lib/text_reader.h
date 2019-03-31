#ifndef __core_text_reader_h_included_
#define __core_text_reader_h_included_

#include <types.h>
#include <stdio.h>

namespace lib {

class text_reader
{
public:
	text_reader( ) = default;

	text_reader( pointer const data, uptr const size );
	void create( pointer const data, uptr const size );

	template<typename T>
	T read( );
	void read_str( pstr const data );
	template<typename ... Args>
	void read_mask( pcstr const mask, Args* const ... arg );
	void skip_line( );
	
	pointer ptr( ) const;

	bool is_valid( ) const;

private:
	pointer	m_pointer;
	pointer m_end;
};

template<typename T>
inline T text_reader::read( )
{
	static_assert( false, "unsupported type for text reader!" );
}

#define READ_PROC( type, mask ) \
template<> \
inline type text_reader::read( ) \
{ \
	type data; \
	read_mask( mask, &data ); \
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

#define TEXT_READER_MASK( mask ) mask "%n"

template<typename ... Args>
void text_reader::read_mask( pcstr const mask, Args* const ... args )
{
	u32 pos;
	_snscanf( m_pointer, m_end - m_pointer, mask, args ..., &pos );
	m_pointer += pos;
}

} // namespace lib

#endif // #ifndef __core_text_reader_h_included_