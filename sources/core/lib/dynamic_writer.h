#ifndef GUARD_CORE_DYNAMIC_WRITER_H_INCLUDED
#define GUARD_CORE_DYNAMIC_WRITER_H_INCLUDED

#include <types.h>
#include "linear_allocator.h"

namespace lib {

class dynamic_writer
{
public:
	dynamic_writer( ) = default;

	void create( );
	void destroy( );

	void operator+=( uptr const size );

	template<typename T>
	inline void write( T const& value );
	
	void write_data( pcvoid const data, uptr const size );
	void write_str( pcstr const value );
	
	pointer data( ) const;
	pointer ptr( ) const;
	uptr size( ) const;

private:
	enum { max_page_count = 16 * Kb };

	dynamic_linear_allocator<Memory_Page_Size, max_page_count> m_allocator;

};

template<typename T>
void dynamic_writer::write( T const& value )
{
	write_data		( &value, sizeof(T) );
}

} // namespace lib

#endif // #ifndef GUARD_CORE_DYNAMIC_WRITER_H_INCLUDED