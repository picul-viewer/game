#ifndef __core_matrix_h_included_
#define __core_matrix_h_included_

#include <core/types.h>
#include <math.h>
#include "vector.h"

namespace math {

struct float2x2;
struct float3x3;
struct float4x3;
struct float4x4;

struct float2x2
{
	union
	{
		struct { float a00, a01, a10, a11; };
		struct { float2 l0, l1; };
		float m[2][2];
		float data[4];
	};

	inline float2x2( ) { }
	inline float2x2( float2 const& l0, float2 const& l1 ) :
		l0( l0 ), l1( l1 )
	{ }

	inline explicit float2x2( float3x3 const& m );
	inline explicit float2x2( float4x3 const& m );
	inline explicit float2x2( float4x4 const& m );
	
	static inline float2x2 get_zero( ) { return float2x2( float2( 0.0f, 0.0f ), float2( 0.0f, 0.0f ) ); }
	static inline float2x2 get_identity( ) { return float2x2( float2( 1.0f, 0.0f ), float2( 0.0f, 1.0f ) ); }

	inline bool operator==( float2x2 const& m ) const { return ( l0 == m.l0 ) && ( l1 == m.l1 ); }
	inline bool operator!=( float2x2 const& m ) const { return ( l0 != m.l0 ) || ( l1 != m.l1 ); }
	
	inline float2x2 operator-( ) const { return float2x2( -l0, -l1 ); }

	inline float2x2 operator+( float2x2 const& v ) const { return float2x2( l0 + v.l0, l1 + v.l1 ); }
	inline float2x2 operator-( float2x2 const& v ) const { return float2x2( l0 - v.l0, l1 - v.l1 ); }
	
	inline float2x2 operator+( float v ) const { return float2x2( l0 + v, l1 + v ); }
	inline float2x2 operator-( float v ) const { return float2x2( l0 - v, l1 - v ); }
	inline float2x2 operator*( float v ) const { return float2x2( l0 * v, l1 * v ); }
	inline float2x2 operator/( float v ) const { return float2x2( l0 / v, l1 / v ); }

	inline float2x2& operator+=( float2x2 const& v ) { l0 += v.l0; l1 += v.l1; return *this; }
	inline float2x2& operator-=( float2x2 const& v ) { l0 -= v.l0; l1 -= v.l1; return *this; }
	
	inline float2x2& operator+=( float v ) { l0 += v; l1 += v; return *this; }
	inline float2x2& operator-=( float v ) { l0 -= v; l1 -= v; return *this; }
	inline float2x2& operator*=( float v ) { l0 *= v; l1 *= v; return *this; }
	inline float2x2& operator/=( float v ) { l0 /= v; l1 /= v; return *this; }

	inline float determinant( ) const { return a00 * a11 - a01 * a10; }

	inline float2x2 inverse( ) const
	{
		const float det = determinant( );
		return float2x2( float2( a11, -a10 ),
						 float2( -a01, a00 ) ) / det;
	}

	inline float2x2 transpose( ) const
	{
		return float2x2( float2( a00, a10 ),
						 float2( a01, a11 ) );
	}
};

struct float3x3
{
	union
	{
		struct { float a00, a01, a02, a10, a11, a12, a20, a21, a22; };
		struct { float3 l0, l1, l2; };
		float m[3][3];
		float data[9];
	};

	inline float3x3( ) { }
	inline float3x3( float3 const& l0, float3 const& l1, float3 const& l2 ) :
		l0( l0 ), l1( l1 ), l2( l2 )
	{ }

	inline explicit float3x3( float2x2 const& m );
	inline explicit float3x3( float4x3 const& m );
	inline explicit float3x3( float4x4 const& m );
	
	static inline float3x3 get_zero( ) { return float3x3( float3( 0.0f, 0.0f, 0.0f ), float3( 0.0f, 0.0f, 0.0f ), float3( 0.0f, 0.0f, 0.0f ) ); }
	static inline float3x3 get_identity( ) { return float3x3( float3( 1.0f, 0.0f, 0.0f ), float3( 0.0f, 1.0f, 0.0f ), float3( 0.0f, 0.0f, 1.0f ) ); }

	inline bool operator==( float3x3 const& v ) const { return ( l0 == v.l0 ) && ( l1 == v.l1 ) && ( l2 == v.l2 ); }
	inline bool operator!=( float3x3 const& v ) const { return ( l0 != v.l0 ) || ( l1 != v.l1 ) || ( l2 != v.l2 ); }
	
	inline float3x3 operator-( ) const { return float3x3( -l0, -l1, -l2 ); }

	inline float3x3 operator+( float3x3 const& v ) const { return float3x3( l0 + v.l0, l1 + v.l1, l2 + v.l2 ); }
	inline float3x3 operator-( float3x3 const& v ) const { return float3x3( l0 - v.l0, l1 - v.l1, l2 - v.l2 ); }
	
	inline float3x3 operator+( float v ) const { return float3x3( l0 + v, l1 + v, l2 + v ); }
	inline float3x3 operator-( float v ) const { return float3x3( l0 - v, l1 - v, l2 - v ); }
	inline float3x3 operator*( float v ) const { return float3x3( l0 * v, l1 * v, l2 * v ); }
	inline float3x3 operator/( float v ) const { return float3x3( l0 / v, l1 / v, l2 / v ); }

	inline float3x3& operator+=( float3x3 const& v ) { l0 += v.l0; l1 += v.l1; l2 += v.l2; return *this; }
	inline float3x3& operator-=( float3x3 const& v ) { l0 -= v.l0; l1 -= v.l1; l2 -= v.l2; return *this; }
	
	inline float3x3& operator+=( float v ) { l0 += v; l1 += v; l2 += v; return *this; }
	inline float3x3& operator-=( float v ) { l0 -= v; l1 -= v; l2 -= v; return *this; }
	inline float3x3& operator*=( float v ) { l0 *= v; l1 *= v; l2 *= v; return *this; }
	inline float3x3& operator/=( float v ) { l0 /= v; l1 /= v; l2 /= v; return *this; }

	inline float determinant( )
	{
		return	a00 * ( a11 * a22 - a12 * a21 ) - 
				a01 * ( a10 * a22 - a12 * a20 ) +
				a02 * ( a10 * a21 - a11 * a20 );
	}

	inline float3x3 inverse( )
	{
		const float det = determinant( );

		return float3x3( float3( ( a11 * a22 - a12 * a21 ), ( a02 * a21 - a01 * a22 ), ( a01 * a12 - a02 * a11 ) ),
						 float3( ( a12 * a20 - a10 * a22 ), ( a00 * a22 - a02 * a20 ), ( a02 * a10 - a00 * a12 ) ),
						 float3( ( a10 * a21 - a11 * a20 ), ( a01 * a20 - a00 * a21 ), ( a00 * a11 - a01 * a10 ) ) ) / det;
	}
	
	inline float3x3 transpose( ) const
	{
		return float3x3( float3( a00, a10, a20 ),
						 float3( a01, a11, a21 ),
						 float3( a02, a12, a22 ) );
	}
};

struct float4x4
{
	union
	{
		struct { float a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22, a23, a30, a31, a32, a33; };
		struct { float4 l0, l1, l2, l3; };
		float m[4][4];
		float data[16];
	};

	inline float4x4() { }
	inline float4x4( float4 const& l0, float4 const& l1, float4 const& l2, float4 const& l3 ) :
		l0( l0 ), l1( l1 ), l2( l2 ), l3( l3 )
	{ }

	inline explicit float4x4( float2x2 const& m );
	inline explicit float4x4( float3x3 const& m );
	inline explicit float4x4( float4x3 const& m );

	static inline float4x4 get_zero( ) { return float4x4( float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ) ); }
	static inline float4x4 get_identity( ) { return float4x4( float4( 1.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 1.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 1.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 1.0f ) ); }
	
	inline bool operator==( float4x4 const& v ) const { return ( l0 == v.l0 ) && ( l1 == v.l1 ) && ( l2 == v.l2 ) && ( l3 == v.l3 ); }
	inline bool operator!=( float4x4 const& v ) const { return ( l0 != v.l0 ) || ( l1 != v.l1 ) || ( l2 != v.l2 ) || ( l3 != v.l3 ); }
	
	inline float4x4 operator-( ) const { return float4x4( -l0, -l1, -l2, -l3 ); }

	inline float4x4 operator+( float4x4 const& v ) const { return float4x4( l0 + v.l0, l1 + v.l1, l2 + v.l2, l3 + v.l3 ); }
	inline float4x4 operator-( float4x4 const& v ) const { return float4x4( l0 - v.l0, l1 - v.l1, l2 - v.l2, l3 - v.l3 ); }
	
	inline float4x4 operator+( float v ) const { return float4x4( l0 + v, l1 + v, l2 + v, l3 + v ); }
	inline float4x4 operator-( float v ) const { return float4x4( l0 - v, l1 - v, l2 - v, l3 - v ); }
	inline float4x4 operator*( float v ) const { return float4x4( l0 * v, l1 * v, l2 * v, l3 * v ); }
	inline float4x4 operator/( float v ) const { return float4x4( l0 / v, l1 / v, l2 / v, l3 / v ); }

	inline float4x4& operator+=( float4x4 const& v ) { l0 += v.l0; l1 += v.l1; l2 += v.l2; l3 += v.l3; return *this; }
	inline float4x4& operator-=( float4x4 const& v ) { l0 -= v.l0; l1 -= v.l1; l2 -= v.l2; l3 -= v.l3; return *this; }
	
	inline float4x4& operator+=( float v ) { l0 += v; l1 += v; l2 += v; l3 += v; return *this; }
	inline float4x4& operator-=( float v ) { l0 -= v; l1 -= v; l2 -= v; l3 -= v; return *this; }
	inline float4x4& operator*=( float v ) { l0 *= v; l1 *= v; l2 *= v; l3 *= v; return *this; }
	inline float4x4& operator/=( float v ) { l0 /= v; l1 /= v; l2 /= v; l3 /= v; return *this; }

	inline float determinant( ) const
	{
		const float
			A = a22 * a33 - a23 * a32,
			B = a21 * a33 - a23 * a31,
			C = a21 * a32 - a22 * a31,
			D = a20 * a33 - a23 * a30,
			E = a20 * a32 - a22 * a30,
			F = a20 * a31 - a21 * a30;

		return	a00 * ( A * a11 - B * a12 + C * a13 ) - 
				a01 * ( A * a10 - D * a12 + E * a13 ) + 
				a02 * ( B * a10 - D * a11 + F * a13 ) - 
				a03 * ( C * a10 - E * a11 + F * a12 );
	}

	inline float4x4 inverse( ) const
	{
		// https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform

		float const s0 = a00 * a11 - a10 * a01;
		float const s1 = a00 * a12 - a10 * a02;
		float const s2 = a00 * a13 - a10 * a03;
		float const s3 = a01 * a12 - a11 * a02;
		float const s4 = a01 * a13 - a11 * a03;
		float const s5 = a02 * a13 - a12 * a03;

		float const c5 = a22 * a33 - a32 * a23;
		float const c4 = a21 * a33 - a31 * a23;
		float const c3 = a21 * a32 - a31 * a22;
		float const c2 = a20 * a33 - a30 * a23;
		float const c1 = a20 * a32 - a30 * a22;
		float const c0 = a20 * a31 - a30 * a21;

		float const invdet = 1 / ( s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0 );

		return float4x4( float4( a11 * c5 - a12 * c4 + a13 * c3,
								 -a01 * c5 + a02 * c4 - a03 * c3,
								 a31 * s5 - a32 * s4 + a33 * s3,
								 -a21 * s5 + a22 * s4 - a23 * s3 ),
						 float4( -a10 * c5 + a12 * c2 - a13 * c1,
								 a00 * c5 - a02 * c2 + a03 * c1,
								 -a30 * s5 + a32 * s2 - a33 * s1,
								 a20 * s5 - a22 * s2 + a23 * s1 ),
						 float4( a10 * c4 - a11 * c2 + a13 * c0,
								 -a00 * c4 + a01 * c2 - a03 * c0,
								 a30 * s4 - a31 * s2 + a33 * s0,
								 -a20 * s4 + a21 * s2 - a23 * s0 ),
						 float4( -a10 * c3 + a11 * c1 - a12 * c0,
								 a00 * c3 - a01 * c1 + a02 * c0,
								 -a30 * s3 + a31 * s1 - a32 * s0,
								 a20 * s3 - a21 * s1 + a22 * s0 ) );
	}

	inline float4x4 transpose( )
	{
		return float4x4( float4( a00, a10, a20, a30 ),
						 float4( a01, a11, a21, a31 ),
						 float4( a02, a12, a22, a32 ),
						 float4( a03, a13, a23, a33 ) );
	}
};

// float4x4 with assumption that last row is ( 0, 0, 0, 1 ).
// Good for transforms.
struct float4x3
{
	union
	{
		struct { float a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22, a23; };
		struct { float4 l0, l1, l2; };
		float m[3][4];
		float data[12];
	};

	inline float4x3() { }
	inline float4x3( float4 const& l0, float4 const& l1, float4 const& l2 ) :
		l0( l0 ), l1( l1 ), l2( l2 )
	{ }

	inline explicit float4x3( float2x2 const& m );
	inline explicit float4x3( float3x3 const& m );
	inline explicit float4x3( float4x4 const& m );

	inline operator float4x4( )
	{
		return float4x4( l0, l1, l2, float4( 0.0f, 0.0f, 0.0f, 1.0f ) );
	}

	static inline float4x3 get_zero( ) { return float4x3( float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ) ); }
	static inline float4x3 get_identity( ) { return float4x3( float4( 1.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 1.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 1.0f, 0.0f ) ); }
	
	inline bool operator==( float4x3 const& v ) const { return ( l0 == v.l0 ) && ( l1 == v.l1 ) && ( l2 == v.l2 ); }
	inline bool operator!=( float4x3 const& v ) const { return ( l0 != v.l0 ) || ( l1 != v.l1 ) || ( l2 != v.l2 ); }
	
	inline float4x3 operator-( ) const { return float4x3( -l0, -l1, -l2 ); }

	inline float4x3 operator+( float4x3 const& v ) const { return float4x3( l0 + v.l0, l1 + v.l1, l2 + v.l2 ); }
	inline float4x3 operator-( float4x3 const& v ) const { return float4x3( l0 - v.l0, l1 - v.l1, l2 - v.l2 ); }
	
	inline float4x3 operator+( float v ) const { return float4x3( l0 + v, l1 + v, l2 + v ); }
	inline float4x3 operator-( float v ) const { return float4x3( l0 - v, l1 - v, l2 - v ); }
	inline float4x3 operator*( float v ) const { return float4x3( l0 * v, l1 * v, l2 * v ); }
	inline float4x3 operator/( float v ) const { return float4x3( l0 / v, l1 / v, l2 / v ); }

	inline float4x3& operator+=( float4x3 const& v ) { l0 += v.l0; l1 += v.l1; l2 += v.l2; return *this; }
	inline float4x3& operator-=( float4x3 const& v ) { l0 -= v.l0; l1 -= v.l1; l2 -= v.l2; return *this; }
	
	inline float4x3& operator+=( float v ) { l0 += v; l1 += v; l2 += v; return *this; }
	inline float4x3& operator-=( float v ) { l0 -= v; l1 -= v; l2 -= v; return *this; }
	inline float4x3& operator*=( float v ) { l0 *= v; l1 *= v; l2 *= v; return *this; }
	inline float4x3& operator/=( float v ) { l0 /= v; l1 /= v; l2 /= v; return *this; }

	inline float determinant( ) const
	{
		const float
			A = a22,
			B = a21,
			D = a20;

		return	a00 * ( a22 * a11 - a21 * a12 ) - 
				a01 * ( a22 * a10 - a20 * a12 ) + 
				a02 * ( a21 * a10 - a20 * a11 );
	}

	inline float4x3 inverse( ) const
	{
		// https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform

		float const s0 = a00 * a11 - a10 * a01;
		float const s1 = a00 * a12 - a10 * a02;
		float const s2 = a00 * a13 - a10 * a03;
		float const s3 = a01 * a12 - a11 * a02;
		float const s4 = a01 * a13 - a11 * a03;
		float const s5 = a02 * a13 - a12 * a03;

		float const c5 = a22;
		float const c4 = a21;
		float const c2 = a20;

		float const invdet = 1 / ( s0 * c5 - s1 * c4 + s3 * c2 );

		return float4x3( float4( a11 * c5 - a12 * c4,
								 -a01 * c5 + a02 * c4,
								 s3,
								 -a21 * s5 + a22 * s4 - a23 * s3 ),
						 float4( -a10 * c5 + a12 * c2,
								 a00 * c5 - a02 * c2,
								 s1,
								 a20 * s5 - a22 * s2 + a23 * s1 ),
						 float4( a10 * c4 - a11 * c2,
								 -a00 * c4 + a01 * c2,
								 s0,
								 -a20 * s4 + a21 * s2 - a23 * s0 ) );
	}
};

float2x2::float2x2( float3x3 const& m ) : l0( m.l0 ), l1( m.l1 ) { }
float2x2::float2x2( float4x3 const& m ) : l0( m.l0 ), l1( m.l1 ) { }
float2x2::float2x2( float4x4 const& m ) : l0( m.l0 ), l1( m.l1 ) { }
float3x3::float3x3( float2x2 const& m ) : l0( m.l0, 0.0f ), l1( m.l1, 0.0f ), l2( 0.0f, 0.0f, 1.0f ) { }
float3x3::float3x3( float4x3 const& m ) : l0( m.l0 ), l1( m.l1 ), l2( m.l2 ) { }
float3x3::float3x3( float4x4 const& m ) : l0( m.l0 ), l1( m.l1 ), l2( m.l2 ) { }
float4x4::float4x4( float2x2 const& m ) : l0( m.l0, 0.0f, 0.0f ), l1( m.l1, 0.0f, 0.0f ), l2( 0.0f, 0.0f, 1.0f, 0.0f ), l3( 0.0f, 0.0f, 0.0f, 1.0f ) { }
float4x4::float4x4( float3x3 const& m ) : l0( m.l0, 0.0f ), l1( m.l1, 0.0f ), l2( m.l2, 0.0f ), l3( 0.0f, 0.0f, 0.0f, 1.0f ) { }
float4x4::float4x4( float4x3 const& m ) : l0( m.l0 ), l1( m.l1 ), l2( m.l2 ), l3( 0.0f, 0.0f, 0.0f, 1.0f ) { }
float4x3::float4x3( float2x2 const& m ) : l0( m.l0 ), l1( m.l1 ), l2( 0.0f, 0.0f, 1.0f, 0.0f ) { }
float4x3::float4x3( float3x3 const& m ) : l0( m.l0 ), l1( m.l1 ), l2( m.l2 ) { }
float4x3::float4x3( float4x4 const& m ) : l0( m.l0 ), l1( m.l1 ), l2( m.l2 ) { }

inline float2 mul( float2x2 const& m, float2 const& v ) { return float2( m.l0.dot( v ), m.l1.dot( v ) ); }
inline float3 mul( float3x3 const& m, float3 const& v ) { return float3( m.l0.dot( v ), m.l1.dot( v ), m.l2.dot( v ) ); }
inline float4 mul( float4x4 const& m, float4 const& v ) { return float4( m.l0.dot( v ), m.l1.dot( v ), m.l2.dot( v ), m.l3.dot( v ) ); }

inline float2 mul( float2 const& v, float2x2 const& m ) { return v.x * m.l0 + v.y * m.l1; }
inline float3 mul( float3 const& v, float3x3 const& m ) { return v.x * m.l0 + v.y * m.l1 + v.z * m.l2; }
inline float4 mul( float4 const& v, float4x4 const& m ) { return v.x * m.l0 + v.y * m.l1 + v.z * m.l2 + v.w * m.l3; }

inline float2x2 mul( float2x2 const& l, float2x2 const& r )
{
	return float2x2( l.l0.x * r.l0 + l.l0.y * r.l1,
					 l.l1.x * r.l0 + l.l1.y * r.l1 );
}
inline float3x3 mul( float3x3 const& l, float3x3 const& r )
{
	return float3x3( l.l0.x * r.l0 + l.l0.y * r.l1 + l.l0.z * r.l2,
					 l.l1.x * r.l0 + l.l1.y * r.l1 + l.l1.z * r.l2,
					 l.l2.x * r.l0 + l.l2.y * r.l1 + l.l2.z * r.l2 );
}
inline float4x4 mul( float4x4 const& l, float4x4 const& r )
{
	return float4x4( l.l0.x * r.l0 + l.l0.y * r.l1 + l.l0.z * r.l2 + l.l0.w * r.l3,
					 l.l1.x * r.l0 + l.l1.y * r.l1 + l.l1.z * r.l2 + l.l1.w * r.l3,
					 l.l2.x * r.l0 + l.l2.y * r.l1 + l.l2.z * r.l2 + l.l2.w * r.l3,
					 l.l3.x * r.l0 + l.l3.y * r.l1 + l.l3.z * r.l2 + l.l3.w * r.l3 );
}

} // namespace math

#endif // #ifndef __core_matrix_h_included_