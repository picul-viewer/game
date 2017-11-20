#ifndef __core_vector_h_included_
#define __core_vector_h_included_

#include <core/types.h>
#include <math.h>

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

	vec2( ) { }
	vec2( T v ) : x( v ), y( v ) { }
	vec2( T x, T y ) : x( x ), y( y ) { }

	template<typename U> operator vec2<U>( ) const { return vec2<U>( (U)x, (U)y ); }
	template<typename U> operator vec3<U>( ) const { return vec3<U>( (U)x, (U)y, (U)0 ); }
	template<typename U> operator vec4<U>( ) const { return vec4<U>( (U)x, (U)y, (U)0, (U)0 ); }
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

	vec3( ) { }
	vec3( T v ) : x( v ), y( v ), z( v ) { }
	vec3( T x, T y, T z ) : x( x ), y( y ), z( z ) { }
	vec3( const vec2<T>& xy, T z ) : vx( xy ), vy( z ) { }

	template<typename U> operator vec2<U>( ) const { return vec2<U>( (U)x, (U)y ); }
	template<typename U> operator vec3<U>( ) const { return vec3<U>( (U)x, (U)y, (U)z ); }
	template<typename U> operator vec4<U>( ) const { return vec4<U>( (U)x, (U)y, (U)z, (U)0 ); }
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
	
	vec4( ) { }
	vec4( T v ) : x( v ), y( v ), z( v ), w( v ) { }
	vec4( T x, T y, T z, T w ) : x( x ), y( y ), z( z ), w( w ) { }
	vec4( const vec2<T>& xy, T z, T w ) : vx( xy.x, z ), vy( w ) { }
	vec4( const vec3<T>& xyz, T w ) : vx( xyz ), vy( w ) { }

	template<typename U> operator vec2<U>( ) const { return vec2<U>( (U)x, (U)y ); }
	template<typename U> operator vec3<U>( ) const { return vec3<U>( (U)x, (U)y, (U)z ); }
	template<typename U> operator vec4<U>( ) const { return vec4<U>( (U)x, (U)y, (U)z, (U)w ); }
};

template<typename T>
bool operator==( T const& l, T const& r ) { return ( l.vx == r.vx ) && ( l.vy == r.vy ); }
template<typename T>
bool operator!=( T const& l, T const& r ) { return ( l.vx != r.vx ) || ( l.vy != r.vy ); }

template<typename T>
bool operator>( T const& l, T const& r ) { return ( l.vx > r.vx ) && ( l.vy > r.vy ); }
template<typename T>
bool operator<( T const& l, T const& r ) { return ( l.vx < r.vx ) && ( l.vy < r.vy ); }
template<typename T>
bool operator>=( T const& l, T const& r ) { return ( l.vx >= r.vx ) && ( l.vy >= r.vy ); }
template<typename T>
bool operator<=( T const& l, T const& r ) { return ( l.vx <= r.vx ) && ( l.vy <= r.vy ); }

template<typename T>
bool operator>( T const& l, typename T::value_type r ) { return ( l.vx > r ) && ( l.vy > r ); }
template<typename T>
bool operator<( T const& l, typename T::value_type r ) { return ( l.vx < r ) && ( l.vy < r ); }
template<typename T>
bool operator>=( T const& l, typename T::value_type r ) { return ( l.vx >= r ) && ( l.vy >= r ); }
template<typename T>
bool operator<=( T const& l, typename T::value_type r ) { return ( l.vx <= r ) && ( l.vy <= r ); }

template<typename T>
T operator-( T const& v ) { return T( -v.vx, -v.vy ); }

template<typename T>
T operator+( T const& l, T const& r ) { return T( l.vx + r.vx, l.vy + r.vy ); }
template<typename T>
T operator-( T const& l, T const& r ) { return T( l.vx - r.vx, l.vy - r.vy ); }
template<typename T>
T operator*( T const& l, T const& r ) { return T( l.vx * r.vx, l.vy * r.vy ); }
template<typename T>
T operator/( T const& l, T const& r ) { return T( l.vx / r.vx, l.vy / r.vy ); }

template<typename T>
T operator+( T const& l, typename T::value_type r ) { return T( l.vx + r, l.vy + r ); }
template<typename T>
T operator-( T const& l, typename T::value_type r ) { return T( l.vx - r, l.vy - r ); }
template<typename T>
T operator*( T const& l, typename T::value_type r ) { return T( l.vx * r, l.vy * r ); }
template<typename T>
T operator/( T const& l, typename T::value_type r ) { return T( l.vx / r, l.vy / r ); }

template<typename T>
T operator+( typename T::value_type l, T const& r ) { return T( l + r.vx, l + r.vy ); }
template<typename T>
T operator-( typename T::value_type l, T const& r ) { return T( l - r.vx, l - r.vy ); }
template<typename T>
T operator*( typename T::value_type l, T const& r ) { return T( l * r.vx, l * r.vy ); }
template<typename T>
T operator/( typename T::value_type l, T const& r ) { return T( l / r.vx, l / r.vy ); }

template<typename T>
T& operator+=( T& l, T const& r ) { l.vx += r.vx; l.vy += r.vy; return l; }
template<typename T>
T& operator-=( T& l, T const& r ) { l.vx -= r.vx; l.vy -= r.vy; return l; }
template<typename T>
T& operator*=( T& l, T const& r ) { l.vx *= r.vx; l.vy *= r.vy; return l; }
template<typename T>
T& operator/=( T& l, T const& r ) { l.vx /= r.vx; l.vy /= r.vy; return l; }

template<typename T>
T& operator+=( T& l, typename T::value_type r ) { l.vx += r; l.vy += r; return l; }
template<typename T>
T& operator-=( T& l, typename T::value_type r ) { l.vx -= r; l.vy -= r; return l; }
template<typename T>
T& operator*=( T& l, typename T::value_type r ) { l.vx *= r; l.vy *= r; return l; }
template<typename T>
T& operator/=( T& l, typename T::value_type r ) { l.vx /= r; l.vy /= r; return l; }

template<>
struct vec2<float>
{
	typedef float value_type;

	union
	{
		struct { float x, y; };
		struct { float vx; float vy; };
		float data[2];
	};
	
	vec2( ) { }
	vec2( float v ) : x( v ), y( v ) { }
	vec2( float x, float y ) : x( x ), y( y ) { }

	template<typename U> operator vec2<U>( ) const { return vec2<U>( (U)x, (U)y ); }
	template<typename U> operator vec3<U>( ) const { return vec3<U>( (U)x, (U)y, (U)0.0f ); }
	template<typename U> operator vec4<U>( ) const { return vec4<U>( (U)x, (U)y, (U)0.0f, (U)0.0f ); }

	inline float dot( vec2<float> const& v ) const { return x * v.x + y * v.y; }

	inline float squared_length( ) const { return dot( *this ); }
	inline float length( ) const { return sqrt( squared_length( ) ); }
	inline vec2<float> normalize( ) const { return *this / length( ); }
	
	inline float cross( vec2<float> const& v ) const { return x * v.y - y * v.x; }
};

template<>
struct vec3<float>
{
	typedef float value_type;
	
	union
	{
		struct { float x, y, z; };
		struct { vec2<float> vx; float vy; };
		float data[3];
	};

	vec3( ) { }
	vec3( float v ) : x( v ), y( v ), z( v ) { }
	vec3( float x, float y, float z ) : x( x ), y( y ), z( z ) { }
	vec3( const vec2<float>& xy, float z ) : vx( xy ), vy( z ) { }

	template<typename U> operator vec2<U>( ) const { return vec2<U>( (U)x, (U)y ); }
	template<typename U> operator vec3<U>( ) const { return vec3<U>( (U)x, (U)y, (U)z ); }
	template<typename U> operator vec4<U>( ) const { return vec4<U>( (U)x, (U)y, (U)z, (U)0 ); }

	inline float dot( vec3<float> const& v ) const { return x * v.x + y * v.y + z * v.z; }

	inline float squared_length( ) const { return dot( *this ); }
	inline float length( ) const { return sqrt( squared_length( ) ); }
	inline vec3<float> normalize( ) const { return *this / length( ); }

	inline vec3<float> cross( vec3<float> const& v ) const { return vec3<float>( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
};

template<>
struct vec4<float>
{
	typedef float value_type;
	
	union
	{
		struct { float x, y, z, w; };
		struct { vec3<float> vx; float vy; };
		float data[4];
	};
	
	vec4( ) { }
	vec4( float v ) : x( v ), y( v ), z( v ), w( v ) { }
	vec4( float x, float y, float z, float w ) : x( x ), y( y ), z( z ), w( w ) { }
	vec4( const vec2<float>& xy, float z, float w ) : vx( xy.x, z ), vy( w ) { }
	vec4( const vec3<float>& xyz, float w ) : vx( xyz ), vy( w ) { }

	template<typename U> operator vec2<U>( ) const { return vec2<U>( (U)x, (U)y ); }
	template<typename U> operator vec3<U>( ) const { return vec3<U>( (U)x, (U)y, (U)z ); }
	template<typename U> operator vec4<U>( ) const { return vec4<U>( (U)x, (U)y, (U)z, (U)w ); }

	inline float dot( vec4<float> const& v ) const { return x * v.x + y * v.y + z * v.z + w * v.w; }

	inline float squared_length( ) const { return dot( *this ); }
	inline float length( ) const { return sqrt( squared_length( ) ); }
	inline vec4<float> normalize( ) const { return *this / length( ); }
};

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

using float2 = vec2<f32>;
using float3 = vec3<f32>;
using float4 = vec4<f32>;
using double2 = vec2<f64>;
using double3 = vec3<f64>;
using double4 = vec4<f64>;

} // namespace math

#endif // #ifndef __core_vector_h_included_