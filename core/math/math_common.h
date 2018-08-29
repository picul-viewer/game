#ifndef __core_math_common_h_included_
#define __core_math_common_h_included_

#include <types.h>

namespace math
{

template<typename T>
inline T min( T a, T b )
{
	return ( a < b ) ? a : b;
}

template<typename T>
inline T max( T a, T b )
{
	return ( a > b ) ? a : b;
}

// For vectors
template<template<typename> typename V, typename T>
inline V<T> min( V<T> const& a, V<T> const& b )
{
	return V<T>( min( a.vx, b.vx ), ( a.vy < b.vy ) ? a.vy : b.vy );
}

template<template<typename> typename V, typename T>
inline V<T> max( V<T> const& a, V<T> const& b )
{
	return V<T>( max( a.vx, b.vx ), ( a.vy > b.vy ) ? a.vy : b.vy );
}

inline float abs( float n )
{
	return ( n >= 0.0f ) ? n : -n;
}

inline double abs( double n )
{
	return ( n >= 0.0 ) ? n : -n;
}

inline float sqrt( float n )
{
	return sqrtf( n );
}

inline double sqrt( double n )
{
	return ::sqrt( n );
}

inline float sin( float n )
{
	return sinf( n );
}

inline double sin( double n )
{
	return ::sin( n );
}

inline float cos( float n )
{
	return cosf( n );
}

inline double cos( double n )
{
	return ::cos( n );
}

inline float tan( float n )
{
	return tanf( n );
}

inline double tan( double n )
{
	return ::tan( n );
}

// returns n * ( 2 ^ power )
inline float const mul_by_pot( float n, u32 power )
{
	u32 res = *(u32*)&n + ( power << 23 );
	return *(float*)&res;
}

// returns n * ( 2 ^ power )
inline double const mul_by_pot( double n, u32 power )
{
	u64 res = *(u64*)&n + ( (u64)power << 52 );
	return *(double*)&res;
}

struct half
{
public:
	inline half( ) { }

	inline half( float n )
	{
		u32 x = *(u32*)&n;
		u16 h = ( ( x >> 16 ) & 0x8000 ) | ( ( ( ( x & 0x7F800000 ) - 0x38000000) >> 13 ) & 0x7C00 ) | ( ( x >> 13 ) & 0x03FF );
	}
	
	inline operator float( )
	{
		u32 res = ( ( data & 0x8000 ) << 16 ) | ( ( ( data & 0x7C00 ) + 0x1C000 ) << 13 ) | ( ( data & 0x03FF ) << 13 );
		return *(float*)&res;
	}
	
	u16 data;
};

} // namespace math

#endif // #ifndef __core_math_common_h_included_