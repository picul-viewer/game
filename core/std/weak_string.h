#ifndef __core_weak_string_h_included_
#define __core_weak_string_h_included_

#include <core/types.h>
#include "basic_string.h"

class weak_const_string : public i_const_string
{
public:
	weak_const_string( ) = default;
	weak_const_string( const char* str );
	weak_const_string( i_const_string const& str );

	~weak_const_string( ) = default;
	
	virtual const char* c_str( ) const override;

protected:
	char const* m_data;
};

class weak_string : public i_string
{
public:
	weak_string( ) = default;
	weak_string( char* str );
	weak_string( i_string const& str );

	~weak_string( ) = default;
	
	virtual char* data( ) const override;
	virtual const char* c_str( ) const override;

protected:
	char* m_data;
};

#endif // #ifndef __core_weak_string_h_included_
