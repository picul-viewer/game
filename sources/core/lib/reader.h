#ifndef GUARD_CORE_READER_H_INCLUDED
#define GUARD_CORE_READER_H_INCLUDED

#include <types.h>

namespace lib {

class reader
{
public:
	reader( ) = default;

	reader( pointer const data, uptr const size );
	void create( pointer const data, uptr const size );

	void operator+=( uptr const size );

	template<typename T>
	inline T const& read( );
	
	pvoid read_data( uptr const size );
	pstr read_str( );
	
	pointer ptr( ) const;

	bool is_valid( ) const;

private:
	pointer	m_pointer;
	pointer m_end;

};

template<typename T>
T const& reader::read( )
{
	T* const data	= (T*)read_data( sizeof(T) );
	return			*data;
}

} // namespace lib

#endif // #ifndef GUARD_CORE_READER_H_INCLUDED