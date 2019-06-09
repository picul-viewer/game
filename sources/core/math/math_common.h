#ifndef GUARD_CORE_MATH_COMMON_H_INCLUDED
#define GUARD_CORE_MATH_COMMON_H_INCLUDED

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

template<typename T>
inline T clamp( T v, T l, T r )
{
	return max( min( v, r ), l );
}

// For vectors
template<template<typename> typename V, typename T>
inline V<T> min( V<T> const& a, V<T> const& b )
{
	return V<T>( min( a.vx, b.vx ), min( a.vy, b.vy ) );
}

template<template<typename> typename V, typename T>
inline V<T> max( V<T> const& a, V<T> const& b )
{
	return V<T>( max( a.vx, b.vx ), max( a.vy, b.vy ) );
}

template<template<typename> typename V, typename T>
inline V<T> clamp( V<T> const& v, V<T> const& l, V<T> const& r )
{
	return V<T>( clamp( v.vx, l.vx, r.vx ), clamp( v.vy, l.vy, r.vy ) );
}

inline float modf( float n, float* integral )
{
	return modff( n, integral );
}

inline double modf( double n, double* integral )
{
	return ::modf( n, integral );
}

inline float floor( float n )
{
	return floorf( n );
}

inline double floor( double n )
{
	return ::floor( n );
}

inline float ceil( float n )
{
	return ceilf( n );
}

inline double ceil( double n )
{
	return ::ceil( n );
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

inline float log( float n )
{
	return logf( n );
}

inline double log( double n )
{
	return ::log( n );
}

inline float log2( float n )
{
	return log2f( n );
}

inline double log2( double n )
{
	return ::log2( n );
}

inline float log10( float n )
{
	return log10f( n );
}

inline double log10( double n )
{
	return ::log10( n );
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
	inline half( ) = default;

	inline half( float n )
	{
		u32 x = *(u32*)&n;
		data = ( x == 0 ) ? 0 : ( ( ( x >> 16 ) & 0x8000 ) | ( ( ( ( x & 0x7F800000 ) - 0x38000000) >> 13 ) & 0x7C00 ) | ( ( x >> 13 ) & 0x03FF ) );
	}
	
	inline operator float( )
	{
		u32 res = ( ( data & 0x8000 ) << 16 ) | ( ( ( data & 0x7C00 ) + 0x1C000 ) << 13 ) | ( ( data & 0x03FF ) << 13 );
		return *(float*)&res;
	}
	
	u16 data;
};

template<typename T>
inline T smooth_step( T t )
{
	T const t2 = t * t;
	return t2 * ( t * ( (T)6.0 * t2 + (T)10.0 ) - (T)15.0f * t2 );
}

template<typename T, typename F>
inline T lerp( T l, T r, F t )
{
	return l + t * ( r - l );
}

template<typename T, typename U>
inline T align_down( T const value, U const alignment )
{
	return value - value % alignment;
}

template<typename T, typename U>
inline T align_up( T const value, U const alignment )
{
	return align_down( value + alignment - 1, alignment );
}

} // namespace math

#endif // #ifndef GUARD_CORE_MATH_COMMON_H_INCLUDED