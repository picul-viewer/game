#ifndef __core_weak_string_h_included_
#define __core_weak_string_h_included_

#include <core/types.h>
#include "basic_string.h"

class weak_const_string : public i_const_string<weak_const_string>
{
public:
	weak_const_string( ) = default;
	weak_const_string( const char* str );

	template<typename StringClass>
	weak_const_string( i_const_string<StringClass> const& str ) :
		m_data( str.c_str( ) )
	{ }

	~weak_const_string( ) = default;
	
	const char* c_str( ) const;

protected:
	char const* m_data;
};

class weak_string : public i_string<weak_string>
{
public:
	weak_string( ) = default;
	weak_string( char* str );

	template<typename StringClass>
	inline weak_string( i_string<StringClass> const& str ) :
		m_data( str.data( ) )
	{ }

	~weak_string( ) = default;
	
	char* data( ) const;
	const char* c_str( ) const;

protected:
	char* m_data;
};

#endif // #ifndef __core_weak_string_h_included_
