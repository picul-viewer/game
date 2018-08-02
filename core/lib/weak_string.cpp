#include "weak_string.h"

weak_const_string::weak_const_string( pcstr str ) :
	m_data( str )
{ }

pcstr weak_const_string::c_str( ) const
{
	return m_data;
}


weak_string::weak_string( pstr str ) :
	m_data( str )
{ }

pcstr weak_string::c_str( ) const
{
	return m_data;
}

pstr weak_string::data( )
{
	return m_data;
}