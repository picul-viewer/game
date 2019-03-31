#ifndef __core_writer_h_included_
#define __core_writer_h_included_

#include <types.h>

namespace lib {

class writer
{
public:
	writer( ) = default;

	writer( pointer const data, uptr const size );
	void create( pointer const data, uptr const size );

	void operator+=( uptr const size );

	template<typename T>
	inline void write( T const& value );
	
	void write_data( pcvoid const data, uptr const size );
	void write_str( pcstr const value );
	
	pointer ptr( ) const;
	uptr size( pointer const memory_pointer ) const;

	bool is_valid( ) const;

private:
	pointer	m_pointer;
	pointer m_end;

};

template<typename T>
void writer::write( T const& value )
{
	write_data		( &value, sizeof(T) );
}

} // namespace lib

#endif // #ifndef __core_writer_h_included_