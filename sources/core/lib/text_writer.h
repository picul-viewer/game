#ifndef GUARD_CORE_TEXT_WRITER_H_INCLUDED
#define GUARD_CORE_TEXT_WRITER_H_INCLUDED

#include <types.h>
#include <stdio.h>

namespace lib {

class text_writer
{
public:
	text_writer( ) = default;

	text_writer( pointer const data, uptr const size );
	void create( pointer const data, uptr const size );

	template<typename T>
	void write( T const& value );
	void write_str( pcstr const data );
	template<typename ... Args>
	void write_mask( pcstr const mask, Args const& ... args );
	
	pointer ptr( ) const;
	uptr size( pointer const memory_pointer ) const;

	bool is_valid( ) const;

private:
	pointer	m_pointer;
	pointer m_end;
};

template<typename T>
inline void text_writer::write( T const& value )
{
	static_assert( false, "unsupported type for text writer!" );
}

#define WRITE_PROC( type, mask ) \
template<> \
inline void text_writer::write( type const& value ) \
{ \
	write_mask( mask, value ); \
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

#define TEXT_WRITER_MASK( mask ) mask

template<typename ... Args>
void text_writer::write_mask( pcstr const mask, Args const& ... args )
{
	m_pointer += snprintf( m_pointer, m_end - m_pointer, mask, args ... );
}

} // namespace lib

#endif // #ifndef GUARD_CORE_TEXT_WRITER_H_INCLUDED