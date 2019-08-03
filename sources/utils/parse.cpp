#include "parse.h"

namespace utils {

bool is_space( char const c )
{
	return ( ( c >= 0x09 ) && ( c <= 0x0D ) ) || ( c == 0x20 );
}

bool is_digit( char const c )
{
	return ( c >= '0' ) && ( c <= '9' );
}

bool is_letter( char const c )
{
	char const upper = c & 0xDF;
	return ( upper >= 'A' ) && ( upper <= 'Z' );
}

bool is_identifier_symbol( char const c )
{
	return is_letter( c ) || is_digit( c ) || ( c == '_' );
}

void skip_spaces( pcstr& s )
{
	while ( is_space( *s ) ) ++s;
}

void skip_str( pcstr& s )
{
	char const lim = *s++;

	while ( ( *s != lim ) && ( *s != '\0' ) ) ++s;

	++s;
}

bool parse_value( pcstr& s, bool& value )
{
	if ( *s == 't' )
	{
		if ( ( s[1] != 'r' ) || ( s[2] != 'u' ) || ( s[3] != 'e' ) || is_identifier_symbol( s[4] ) )
			return false;

		s += 4;

		value = true;
	}
	else if ( *s == 'f' )
	{
		if ( ( s[1] != 'a' ) || ( s[2] != 'l' ) || ( s[3] != 's' ) || ( s[4] != 'e' ) || is_identifier_symbol( s[5] ) )
			return false;

		s += 5;

		value = false;
	}

	return true;
}

bool parse_value( pcstr& s, s32& value )
{
	s64 val = value;
	bool const result = parse_value( s, val );
	value = (s32)val;
	return result;
}

bool parse_value( pcstr& s, u32& value )
{
	u64 val = value;
	bool const result = parse_value( s, val );
	value = (u32)val;
	return result;
}

bool parse_value( pcstr& s, s64& value )
{
	bool positive = true;

	if ( *s == '-' )
	{
		positive = false;
		++s;
	}
	else if ( *s == '+' )
		++s;

	s64 num = 0;

	while ( is_digit( *s ) )
	{
		num = num * 10 + *s - '0';
		++s;
	}

	value = positive ? num : -num;

	return true;
}

bool parse_value( pcstr& s, u64& value )
{
	if ( ( *s == '-' ) || ( *s == '+' ) )
		++s;

	u64 num = 0;

	while ( is_digit( *s ) )
	{
		num = num * 10 + *s - '0';
		++s;
	}

	value = num;

	return true;
}

bool parse_value( pcstr& s, float& value )
{
	double val = value;
	bool const result = parse_value( s, val );
	value = (float)val;
	return result;
}

bool parse_value( pcstr& s, double& value )
{
	bool positive = true;

	if ( *s == '-' )
	{
		positive = false;
		++s;
	}
	else if ( *s == '+' )
		++s;

	double num = 0.0;

	while ( is_digit( *s ) )
	{
		num = num * 10.0 + (double)( *s - '0' );
		++s;
	}

	if ( *s == '.' )
		++s;

	double denom = 0.1;

	while ( is_digit( *s ) )
	{
		num += (double)( *s - '0' ) * denom;
		denom *= 0.1;
		++s;
	}

	if ( !positive )
		num = -num;

	if ( ( *s == 'e' ) || ( *s == 'E' ) )
		++s;

	bool exp_positive = true;

	if ( *s == '-' )
	{
		exp_positive = false;
		++s;
	}
	else if ( *s == '+' )
		++s;

	double exp = 1.0;

	constexpr double pows[] = { 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9 };

	while ( is_digit( *s ) )
	{
		double const exp2 = exp * exp;
		double const exp4 = exp2 * exp2;
		exp = exp4 * exp4 * exp2 * pows[*s - '0'];
		++s;
	}

	if ( !exp_positive )
		exp = 1.0 / exp;

	value = (float)( num * exp );

	return true;
}

bool parse_value( pcstr& s, str256& value )
{
	if ( *s++ != '"' )
		return false;

	pcstr const start = s;
	uptr size = 0;

	for ( ; ( size < str256::max_string_size - 1 ) && ( *s != '"' ); ++size, ++s );

	++s;

	strings::copy_n( value.data( ), start, size );
	value[size] = '\0';

	return true;
}

} // namespace utils