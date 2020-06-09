#ifndef GUARD_CORE_VECTOR_H_INCLUDED
#define GUARD_CORE_VECTOR_H_INCLUDED

#include <types.h>
#include <math.h>
#include "math_common.h"

namespace math {

template<typename T>
struct vec2;
template<typename T>
struct vec3;
template<typename T>
struct vec4;

template<typename T>
struct vec2
{
	typedef T value_type;

	union
	{
		struct { T x, y; };
		struct { T vx, vy; };
		T data[2];
	};

	vec2( ) = default;
	vec2( vec2<T> const& v ) : x( v.x ), y( v.y ) { }
	vec2( vec3<T> const& v ) : x( v.x ), y( v.y ) { }
	vec2( vec4<T> const& v ) : x( v.x ), y( v.y ) { }
	explicit vec2( T v ) : x( v ), y( v ) { }
	explicit vec2( T x, T y ) : x( x ), y( y ) { }
	explicit vec2( T* v ) : x( v[0] ), y( v[1] ) { }

	template<typename U> operator vec2<U>( ) const { return vec2<U>( (U)x, (U)y ); }

	vec2<T>& operator+=( vec2<T> const& r );
	vec2<T>& operator-=( vec2<T> const& r );
	vec2<T>& operator*=( vec2<T> const& r );
	vec2<T>& operator/=( vec2<T> const& r );

	vec2<T>& operator+=( T r );
	vec2<T>& operator-=( T r );
	vec2<T>& operator*=( T r );
	vec2<T>& operator/=( T r );
};

template<typename T>
struct vec3
{
	typedef T value_type;

	union
	{
		struct { T x, y, z; };
		struct { vec2<T> vx; T vy; };
		T data[3];
	};

	vec3( ) = default;
	vec3( vec2<T> const& v ) : x( v.x ), y( v.y ), z( 0 ) { }
	vec3( vec3<T> const& v ) : x( v.x ), y( v.y ), z( v.z ) { }
	vec3( vec4<T> const& v ) : x( v.x ), y( v.y ), z( v.z ) { }
	explicit vec3( vec2<T> const& v, T z ) : vx( v ), vy( z ) { }
	explicit vec3( T v ) : x( v ), y( v ), z( v ) { }
	explicit vec3( T x, T y, T z ) : x( x ), y( y ), z( z ) { }
	explicit vec3( T* v ) : x( v[0] ), y( v[1] ), z( v[2] ) { }

	template<typename U> operator vec3<U>( ) const { return vec3<U>( (U)x, (U)y, (U)z ); }

	vec3<T>& operator+=( vec3<T> const& r );
	vec3<T>& operator-=( vec3<T> const& r );
	vec3<T>& operator*=( vec3<T> const& r );
	vec3<T>& operator/=( vec3<T> const& r );

	vec3<T>& operator+=( T r );
	vec3<T>& operator-=( T r );
	vec3<T>& operator*=( T r );
	vec3<T>& operator/=( T r );
};

template<typename T>
struct vec4
{
	typedef T value_type;

	union
	{
		struct { T x, y, z, w; };
		struct { vec3<T> vx; T vy; };
		T data[4];
	};
	
	vec4( ) = default;
	vec4( vec2<T> const& v ) : x( v.x ), y( v.y ), z( 0 ), w( 0 ) { }
	vec4( vec3<T> const& v ) : x( v.x ), y( v.y ), z( v.z ), w( 0 ) { }
	vec4( vec4<T> const& v ) : x( v.x ), y( v.y ), z( v.z ), w( v.w ) { }
	explicit vec4( vec2<T> const& v, T z, T w ) : vx( v, z ), vy( w ) { }
	explicit vec4( vec3<T> const& v, T w ) : vx( v ), vy( w ) { }
	explicit vec4( T v ) : x( v ), y( v ), z( v ), w( v ) { }
	explicit vec4( T x, T y, T z, T w ) : x( x ), y( y ), z( z ), w( w ) { }
	explicit vec4( T* v ) : x( v[0] ), y( v[1] ), z( v[2] ), w( v[3] ) { }

	template<typename U> operator vec4<U>( ) const { return vec4<U>( (U)x, (U)y, (U)z, (U)w ); }

	vec4<T>& operator+=( vec4<T> const& r );
	vec4<T>& operator-=( vec4<T> const& r );
	vec4<T>& operator*=( vec4<T> const& r );
	vec4<T>& operator/=( vec4<T> const& r );

	vec4<T>& operator+=( T r );
	vec4<T>& operator-=( T r );
	vec4<T>& operator*=( T r );
	vec4<T>& operator/=( T r );
};

template<typename T>
bool operator==( vec2<T> const& l, vec2<T> const& r ) { return ( l.vx == r.vx ) && ( l.vy == r.vy ); }
template<typename T>
bool operator!=( vec2<T> const& l, vec2<T> const& r ) { return ( l.vx != r.vx ) || ( l.vy != r.vy ); }

template<typename T>
bool operator>( vec2<T> const& l, vec2<T> const& r ) { return ( l.vx > r.vx ) && ( l.vy > r.vy ); }
template<typename T>
bool operator<( vec2<T> const& l, vec2<T> const& r ) { return ( l.vx < r.vx ) && ( l.vy < r.vy ); }
template<typename T>
bool operator>=( vec2<T> const& l, vec2<T> const& r ) { return ( l.vx >= r.vx ) && ( l.vy >= r.vy ); }
template<typename T>
bool operator<=( vec2<T> const& l, vec2<T> const& r ) { return ( l.vx <= r.vx ) && ( l.vy <= r.vy ); }

template<typename T>
bool operator>( vec2<T> const& l, T r ) { return ( l.vx > r ) && ( l.vy > r ); }
template<typename T>
bool operator<( vec2<T> const& l, T r ) { return ( l.vx < r ) && ( l.vy < r ); }
template<typename T>
bool operator>=( vec2<T> const& l, T r ) { return ( l.vx >= r ) && ( l.vy >= r ); }
template<typename T>
bool operator<=( vec2<T> const& l, T r ) { return ( l.vx <= r ) && ( l.vy <= r ); }

template<typename T>
vec2<T> operator-( vec2<T> const& v ) { return vec2<T>( -v.vx, -v.vy ); }

template<typename T>
vec2<T> operator+( vec2<T> const& l, vec2<T> const& r ) { return vec2<T>( l.vx + r.vx, l.vy + r.vy ); }
template<typename T>
vec2<T> operator-( vec2<T> const& l, vec2<T> const& r ) { return vec2<T>( l.vx - r.vx, l.vy - r.vy ); }
template<typename T>
vec2<T> operator*( vec2<T> const& l, vec2<T> const& r ) { return vec2<T>( l.vx * r.vx, l.vy * r.vy ); }
template<typename T>
vec2<T> operator/( vec2<T> const& l, vec2<T> const& r ) { return vec2<T>( l.vx / r.vx, l.vy / r.vy ); }

template<typename T>
vec2<T> operator+( vec2<T> const& l, T r ) { return vec2<T>( l.vx + r, l.vy + r ); }
template<typename T>
vec2<T> operator-( vec2<T> const& l, T r ) { return vec2<T>( l.vx - r, l.vy - r ); }
template<typename T>
vec2<T> operator*( vec2<T> const& l, T r ) { return vec2<T>( l.vx * r, l.vy * r ); }
template<typename T>
vec2<T> operator/( vec2<T> const& l, T r ) { return vec2<T>( l.vx / r, l.vy / r ); }

template<typename T>
vec2<T> operator+( T l, vec2<T> const& r ) { return vec2<T>( l + r.vx, l + r.vy ); }
template<typename T>
vec2<T> operator-( T l, vec2<T> const& r ) { return vec2<T>( l - r.vx, l - r.vy ); }
template<typename T>
vec2<T> operator*( T l, vec2<T> const& r ) { return vec2<T>( l * r.vx, l * r.vy ); }
template<typename T>
vec2<T> operator/( T l, vec2<T> const& r ) { return vec2<T>( l / r.vx, l / r.vy ); }

template<typename T>
vec2<T>& vec2<T>::operator+=( vec2<T> const& r ) { vx += r.vx; vy += r.vy; return *this; }
template<typename T>
vec2<T>& vec2<T>::operator-=( vec2<T> const& r ) { vx -= r.vx; vy -= r.vy; return *this; }
template<typename T>
vec2<T>& vec2<T>::operator*=( vec2<T> const& r ) { vx *= r.vx; vy *= r.vy; return *this; }
template<typename T>
vec2<T>& vec2<T>::operator/=( vec2<T> const& r ) { vx /= r.vx; vy /= r.vy; return *this; }

template<typename T>
vec2<T>& vec2<T>::operator+=( T r ) { vx += r; vy += r; return *this; }
template<typename T>
vec2<T>& vec2<T>::operator-=( T r ) { vx -= r; vy -= r; return *this; }
template<typename T>
vec2<T>& vec2<T>::operator*=( T r ) { vx *= r; vy *= r; return *this; }
template<typename T>
vec2<T>& vec2<T>::operator/=( T r ) { vx /= r; vy /= r; return *this; }


template<typename T>
bool operator==( vec3<T> const& l, vec3<T> const& r ) { return ( l.vx == r.vx ) && ( l.vy == r.vy ); }
template<typename T>
bool operator!=( vec3<T> const& l, vec3<T> const& r ) { return ( l.vx != r.vx ) || ( l.vy != r.vy ); }

template<typename T>
bool operator>( vec3<T> const& l, vec3<T> const& r ) { return ( l.vx > r.vx ) && ( l.vy > r.vy ); }
template<typename T>
bool operator<( vec3<T> const& l, vec3<T> const& r ) { return ( l.vx < r.vx ) && ( l.vy < r.vy ); }
template<typename T>
bool operator>=( vec3<T> const& l, vec3<T> const& r ) { return ( l.vx >= r.vx ) && ( l.vy >= r.vy ); }
template<typename T>
bool operator<=( vec3<T> const& l, vec3<T> const& r ) { return ( l.vx <= r.vx ) && ( l.vy <= r.vy ); }

template<typename T>
bool operator>( vec3<T> const& l, T r ) { return ( l.vx > r ) && ( l.vy > r ); }
template<typename T>
bool operator<( vec3<T> const& l, T r ) { return ( l.vx < r ) && ( l.vy < r ); }
template<typename T>
bool operator>=( vec3<T> const& l, T r ) { return ( l.vx >= r ) && ( l.vy >= r ); }
template<typename T>
bool operator<=( vec3<T> const& l, T r ) { return ( l.vx <= r ) && ( l.vy <= r ); }

template<typename T>
vec3<T> operator-( vec3<T> const& v ) { return vec3<T>( -v.vx, -v.vy ); }

template<typename T>
vec3<T> operator+( vec3<T> const& l, vec3<T> const& r ) { return vec3<T>( l.vx + r.vx, l.vy + r.vy ); }
template<typename T>
vec3<T> operator-( vec3<T> const& l, vec3<T> const& r ) { return vec3<T>( l.vx - r.vx, l.vy - r.vy ); }
template<typename T>
vec3<T> operator*( vec3<T> const& l, vec3<T> const& r ) { return vec3<T>( l.vx * r.vx, l.vy * r.vy ); }
template<typename T>
vec3<T> operator/( vec3<T> const& l, vec3<T> const& r ) { return vec3<T>( l.vx / r.vx, l.vy / r.vy ); }

template<typename T>
vec3<T> operator+( vec3<T> const& l, T r ) { return vec3<T>( l.vx + r, l.vy + r ); }
template<typename T>
vec3<T> operator-( vec3<T> const& l, T r ) { return vec3<T>( l.vx - r, l.vy - r ); }
template<typename T>
vec3<T> operator*( vec3<T> const& l, T r ) { return vec3<T>( l.vx * r, l.vy * r ); }
template<typename T>
vec3<T> operator/( vec3<T> const& l, T r ) { return vec3<T>( l.vx / r, l.vy / r ); }

template<typename T>
vec3<T> operator+( T l, vec3<T> const& r ) { return vec3<T>( l + r.vx, l + r.vy ); }
template<typename T>
vec3<T> operator-( T l, vec3<T> const& r ) { return vec3<T>( l - r.vx, l - r.vy ); }
template<typename T>
vec3<T> operator*( T l, vec3<T> const& r ) { return vec3<T>( l * r.vx, l * r.vy ); }
template<typename T>
vec3<T> operator/( T l, vec3<T> const& r ) { return vec3<T>( l / r.vx, l / r.vy ); }

template<typename T>
vec3<T>& vec3<T>::operator+=( vec3<T> const& r ) { vx += r.vx; vy += r.vy; return *this; }
template<typename T>
vec3<T>& vec3<T>::operator-=( vec3<T> const& r ) { vx -= r.vx; vy -= r.vy; return *this; }
template<typename T>
vec3<T>& vec3<T>::operator*=( vec3<T> const& r ) { vx *= r.vx; vy *= r.vy; return *this; }
template<typename T>
vec3<T>& vec3<T>::operator/=( vec3<T> const& r ) { vx /= r.vx; vy /= r.vy; return *this; }

template<typename T>
vec3<T>& vec3<T>::operator+=( T r ) { vx += r; vy += r; return *this; }
template<typename T>
vec3<T>& vec3<T>::operator-=( T r ) { vx -= r; vy -= r; return *this; }
template<typename T>
vec3<T>& vec3<T>::operator*=( T r ) { vx *= r; vy *= r; return *this; }
template<typename T>
vec3<T>& vec3<T>::operator/=( T r ) { vx /= r; vy /= r; return *this; }


template<typename T>
bool operator==( vec4<T> const& l, vec4<T> const& r ) { return ( l.vx == r.vx ) && ( l.vy == r.vy ); }
template<typename T>
bool operator!=( vec4<T> const& l, vec4<T> const& r ) { return ( l.vx != r.vx ) || ( l.vy != r.vy ); }

template<typename T>
bool operator>( vec4<T> const& l, vec4<T> const& r ) { return ( l.vx > r.vx ) && ( l.vy > r.vy ); }
template<typename T>
bool operator<( vec4<T> const& l, vec4<T> const& r ) { return ( l.vx < r.vx ) && ( l.vy < r.vy ); }
template<typename T>
bool operator>=( vec4<T> const& l, vec4<T> const& r ) { return ( l.vx >= r.vx ) && ( l.vy >= r.vy ); }
template<typename T>
bool operator<=( vec4<T> const& l, vec4<T> const& r ) { return ( l.vx <= r.vx ) && ( l.vy <= r.vy ); }

template<typename T>
bool operator>( vec4<T> const& l, T r ) { return ( l.vx > r ) && ( l.vy > r ); }
template<typename T>
bool operator<( vec4<T> const& l, T r ) { return ( l.vx < r ) && ( l.vy < r ); }
template<typename T>
bool operator>=( vec4<T> const& l, T r ) { return ( l.vx >= r ) && ( l.vy >= r ); }
template<typename T>
bool operator<=( vec4<T> const& l, T r ) { return ( l.vx <= r ) && ( l.vy <= r ); }

template<typename T>
vec4<T> operator-( vec4<T> const& v ) { return vec4<T>( -v.vx, -v.vy ); }

template<typename T>
vec4<T> operator+( vec4<T> const& l, vec4<T> const& r ) { return vec4<T>( l.vx + r.vx, l.vy + r.vy ); }
template<typename T>
vec4<T> operator-( vec4<T> const& l, vec4<T> const& r ) { return vec4<T>( l.vx - r.vx, l.vy - r.vy ); }
template<typename T>
vec4<T> operator*( vec4<T> const& l, vec4<T> const& r ) { return vec4<T>( l.vx * r.vx, l.vy * r.vy ); }
template<typename T>
vec4<T> operator/( vec4<T> const& l, vec4<T> const& r ) { return vec4<T>( l.vx / r.vx, l.vy / r.vy ); }

template<typename T>
vec4<T> operator+( vec4<T> const& l, T r ) { return vec4<T>( l.vx + r, l.vy + r ); }
template<typename T>
vec4<T> operator-( vec4<T> const& l, T r ) { return vec4<T>( l.vx - r, l.vy - r ); }
template<typename T>
vec4<T> operator*( vec4<T> const& l, T r ) { return vec4<T>( l.vx * r, l.vy * r ); }
template<typename T>
vec4<T> operator/( vec4<T> const& l, T r ) { return vec4<T>( l.vx / r, l.vy / r ); }

template<typename T>
vec4<T> operator+( T l, vec4<T> const& r ) { return vec4<T>( l + r.vx, l + r.vy ); }
template<typename T>
vec4<T> operator-( T l, vec4<T> const& r ) { return vec4<T>( l - r.vx, l - r.vy ); }
template<typename T>
vec4<T> operator*( T l, vec4<T> const& r ) { return vec4<T>( l * r.vx, l * r.vy ); }
template<typename T>
vec4<T> operator/( T l, vec4<T> const& r ) { return vec4<T>( l / r.vx, l / r.vy ); }

template<typename T>
vec4<T>& vec4<T>::operator+=( vec4<T> const& r ) { vx += r.vx; vy += r.vy; return *this; }
template<typename T>
vec4<T>& vec4<T>::operator-=( vec4<T> const& r ) { vx -= r.vx; vy -= r.vy; return *this; }
template<typename T>
vec4<T>& vec4<T>::operator*=( vec4<T> const& r ) { vx *= r.vx; vy *= r.vy; return *this; }
template<typename T>
vec4<T>& vec4<T>::operator/=( vec4<T> const& r ) { vx /= r.vx; vy /= r.vy; return *this; }

template<typename T>
vec4<T>& vec4<T>::operator+=( T r ) { vx += r; vy += r; return *this; }
template<typename T>
vec4<T>& vec4<T>::operator-=( T r ) { vx -= r; vy -= r; return *this; }
template<typename T>
vec4<T>& vec4<T>::operator*=( T r ) { vx *= r; vy *= r; return *this; }
template<typename T>
vec4<T>& vec4<T>::operator/=( T r ) { vx /= r; vy /= r; return *this; }


template<typename T>
inline T dot( vec2<T> const& l, vec2<T> const& r )
{
	return l.x * r.x + l.y * r.y;
}

template<typename T>
inline T dot( vec3<T> const& l, vec3<T> const& r )
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

template<typename T>
inline T dot( vec4<T> const& l, vec4<T> const& r )
{
	return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
}

template<typename T>
inline typename T::value_type squared_length( T const& v )
{
	return dot( v, v );
}

template<typename T>
inline typename T::value_type length( T const& v )
{
	return math::sqrt( squared_length( v ) );
}

namespace __math_vector_detail {

template<typename T>
struct math_floating_check { enum { value = false }; };
template<>
struct math_floating_check<float> { enum { value = true }; };
template<>
struct math_floating_check<double> { enum { value = true }; };

}

// Floating point only
template<typename T>
inline T normalize( T const& v )
{
	static_assert( __math_vector_detail::math_floating_check<typename T::value_type>::value, "this function may be used only for floating-point vectors" );
	return v / length( v );
}

template<typename T>
inline T cross( vec2<T> const& l, vec2<T> const& r )
{
	return l.x * r.y - l.y * r.x;
}

template<typename T>
inline vec3<T> cross( vec3<T> const& l, vec3<T> const& r )
{
	return vec3<T>( l.y * r.z - l.z * r.y,
					l.z * r.x - l.x * r.z,
					l.x * r.y - l.y * r.x );
}

using u8x2  = vec2<u8>;
using u8x3  = vec3<u8>;
using u8x4  = vec4<u8>;
using u16x2 = vec2<u16>;
using u16x3 = vec3<u16>;
using u16x4 = vec4<u16>;
using u32x2 = vec2<u32>;
using u32x3 = vec3<u32>;
using u32x4 = vec4<u32>;
using u64x2 = vec2<u64>;
using u64x3 = vec3<u64>;
using u64x4 = vec4<u64>;

using s8x2  = vec2<s8>;
using s8x3  = vec3<s8>;
using s8x4  = vec4<s8>;
using s16x2 = vec2<s16>;
using s16x3 = vec3<s16>;
using s16x4 = vec4<s16>;
using s32x2 = vec2<s32>;
using s32x3 = vec3<s32>;
using s32x4 = vec4<s32>;
using s64x2 = vec2<s64>;
using s64x3 = vec3<s64>;
using s64x4 = vec4<s64>;

using half2 = vec2<half>;
using half3 = vec3<half>;
using half4 = vec4<half>;
using unorm16x2 = vec2<unorm16>;
using unorm16x3 = vec3<unorm16>;
using unorm16x4 = vec4<unorm16>;
using float2 = vec2<f32>;
using float3 = vec3<f32>;
using float4 = vec4<f32>;
using double2 = vec2<f64>;
using double3 = vec3<f64>;
using double4 = vec4<f64>;

} // namespace math

#endif // #ifndef GUARD_CORE_VECTOR_H_INCLUDED