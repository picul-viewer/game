#include "weak_string.h"

weak_const_string::weak_const_string( const char* str ) :
	m_data( str )
{ }

weak_const_string::weak_const_string( i_const_string const& str ) :
	m_data( str.c_str( ) )
{ }

const char* weak_const_string::c_str( ) const
{
	return m_data;
}


weak_string::weak_string( char* str ) :
	m_data( str )
{ }

weak_string::weak_string( i_string const& str ) :
	m_data( str.data( ) )
{ }

char* weak_string::data( ) const
{
	return m_data;
}

const char* weak_string::c_str( ) const
{
	return m_data;
}