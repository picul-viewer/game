#ifndef GUARD_CORE_WEAK_STRING_H_INCLUDED
#define GUARD_CORE_WEAK_STRING_H_INCLUDED

#include <types.h>
#include "basic_string.h"

class weak_const_string : public i_const_string<weak_const_string>
{
public:
	weak_const_string( ) = default;

	weak_const_string( pcstr str );
	template<typename StringClass>
	weak_const_string( i_const_string<StringClass> const& str );

	~weak_const_string( ) = default;
	
	pcstr c_str( ) const;

protected:
	pcstr m_data;
};

class weak_string : public i_string<weak_string>
{
public:
	weak_string( ) = default;

	weak_string( pstr str );
	template<typename StringClass>
	inline weak_string( i_string<StringClass> const& str );

	~weak_string( ) = default;
	
	pcstr c_str( ) const;
	pstr data( );

protected:
	pstr m_data;
};

#include "weak_string_inline.h"

#endif // #ifndef GUARD_CORE_WEAK_STRING_H_INCLUDED
