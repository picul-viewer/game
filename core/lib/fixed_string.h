#ifndef __core_fixed_string_h_included_
#define __core_fixed_string_h_included_

#include <types.h>
#include "basic_string.h"

template<uptr MaxSize>
class fixed_string : public i_string<fixed_string<MaxSize>>
{
public:
	enum { max_string_size = MaxSize };

	fixed_string( );
	fixed_string( const char* str );
	fixed_string( const char* str, uptr size );

	template<typename StringClass>
	fixed_string( i_const_string<StringClass> const& str );

	~fixed_string( ) = default;
	
	template<typename StringClass>
	fixed_string& operator=( i_const_string<StringClass> const& str );
	template<typename StringClass>
	fixed_string& assign( i_const_string<StringClass> const& str );
	
	template<typename StringClass>
	fixed_string& operator+=( i_const_string<StringClass> const& str );
	template<typename StringClass>
	fixed_string& append( i_const_string<StringClass> const& str );

	char* data( ) const;
	const char* c_str( ) const;

	fixed_string copy( uptr start, uptr length ) const;

protected:
	char	m_data[MaxSize];
};

template<uptr MaxSize, typename StringClass>
fixed_string<MaxSize> operator+( fixed_string<MaxSize> const& l, i_const_string<StringClass> const& r );

typedef fixed_string<Cache_Line>	little_string;
typedef fixed_string<256>			str256;
typedef fixed_string<512>			str512;
typedef fixed_string<1024>			str1024;
typedef fixed_string<2048>			str2048;
typedef fixed_string<4096>			str4096;
typedef fixed_string<8192>			str8192;

#include "fixed_string_inline.h"

#endif // #ifndef __core_fixed_string_h_included_
