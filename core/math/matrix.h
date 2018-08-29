#ifndef __core_matrix_h_included_
#define __core_matrix_h_included_

#include <types.h>
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
	inline float2x2( float2x2 const& m ) :
		l0( m.l0 ), l1( m.l1 )
	{ }
	inline float2x2( float2 const& l0, float2 const& l1 ) :
		l0( l0 ), l1( l1 )
	{ }

	inline explicit float2x2( float3x3 const& m );
	inline explicit float2x2( float4x3 const& m );
	inline explicit float2x2( float4x4 const& m );
	
	static inline float2x2 zero( ) { return float2x2( float2( 0.0f, 0.0f ), float2( 0.0f, 0.0f ) ); }
	static inline float2x2 identity( ) { return float2x2( float2( 1.0f, 0.0f ), float2( 0.0f, 1.0f ) ); }

	inline bool operator==( float2x2 const& m ) const { return ( l0 == m.l0 ) && ( l1 == m.l1 ); }
	inline bool operator!=( float2x2 const& m ) const { return ( l0 != m.l0 ) || ( l1 != m.l1 ); }
	
	inline float2x2 operator-( ) const { return float2x2( -l0, -l1 ); }

	inline float2x2 operator+( float2x2 const& v ) const { return float2x2( l0 + v.l0, l1 + v.l1 ); }
	inline float2x2 operator-( float2x2 const& v ) const { return float2x2( l0 - v.l0, l1 - v.l1 ); }
	
	inline float2x2 operator+( float v ) const { return float2x2( l0 + v, l1 + v ); }
	inline float2x2 operator-( float v ) const { return float2x2( l0 - v, l1 - v ); }
	inline float2x2 operator*( float v ) const { return float2x2( l0 * v, l1 * v ); }
	inline float2x2 operator/( float v ) const
	{
		float const inv_v = 1.0f / v;
		return float2x2( l0 * inv_v, l1 * inv_v );
	}

	inline float2x2& operator+=( float2x2 const& v ) { l0 += v.l0; l1 += v.l1; return *this; }
	inline float2x2& operator-=( float2x2 const& v ) { l0 -= v.l0; l1 -= v.l1; return *this; }
	
	inline float2x2& operator+=( float v ) { l0 += v; l1 += v; return *this; }
	inline float2x2& operator-=( float v ) { l0 -= v; l1 -= v; return *this; }
	inline float2x2& operator*=( float v ) { l0 *= v; l1 *= v; return *this; }
	inline float2x2& operator/=( float v )
	{
		float const inv_v = 1.0f / v;
		l0 *= inv_v;
		l1 *= inv_v;
		return *this;
	}
};

inline float determinant( float2x2 const& m )
{
	return m.a00 * m.a11 - m.a01 * m.a10;
}

inline float2x2 inverse( float2x2 const& m )
{
	float const det = determinant( m );
	return float2x2( float2( m.a11, -m.a10 ),
					 float2( -m.a01, m.a00 ) ) / det;
}

inline float2x2 transpose( float2x2 const& m )
{
	return float2x2( float2( m.a00, m.a10 ),
					 float2( m.a01, m.a11 ) );
}

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
	inline float3x3( float3x3 const& m ) :
		l0( m.l0 ), l1( m.l1 ), l2( m.l2 )
	{ }
	inline float3x3( float3 const& l0, float3 const& l1, float3 const& l2 ) :
		l0( l0 ), l1( l1 ), l2( l2 )
	{ }

	inline explicit float3x3( float2x2 const& m );
	inline explicit float3x3( float4x3 const& m );
	inline explicit float3x3( float4x4 const& m );
	
	static inline float3x3 zero( ) { return float3x3( float3( 0.0f, 0.0f, 0.0f ), float3( 0.0f, 0.0f, 0.0f ), float3( 0.0f, 0.0f, 0.0f ) ); }
	static inline float3x3 identity( ) { return float3x3( float3( 1.0f, 0.0f, 0.0f ), float3( 0.0f, 1.0f, 0.0f ), float3( 0.0f, 0.0f, 1.0f ) ); }

	inline bool operator==( float3x3 const& v ) const { return ( l0 == v.l0 ) && ( l1 == v.l1 ) && ( l2 == v.l2 ); }
	inline bool operator!=( float3x3 const& v ) const { return ( l0 != v.l0 ) || ( l1 != v.l1 ) || ( l2 != v.l2 ); }
	
	inline float3x3 operator-( ) const { return float3x3( -l0, -l1, -l2 ); }

	inline float3x3 operator+( float3x3 const& v ) const { return float3x3( l0 + v.l0, l1 + v.l1, l2 + v.l2 ); }
	inline float3x3 operator-( float3x3 const& v ) const { return float3x3( l0 - v.l0, l1 - v.l1, l2 - v.l2 ); }
	
	inline float3x3 operator+( float v ) const { return float3x3( l0 + v, l1 + v, l2 + v ); }
	inline float3x3 operator-( float v ) const { return float3x3( l0 - v, l1 - v, l2 - v ); }
	inline float3x3 operator*( float v ) const { return float3x3( l0 * v, l1 * v, l2 * v ); }
	inline float3x3 operator/( float v ) const
	{
		float const inv_v = 1.0f / v;
		return float3x3( l0 * inv_v, l1 * inv_v, l2 * inv_v );
	}

	inline float3x3& operator+=( float3x3 const& v ) { l0 += v.l0; l1 += v.l1; l2 += v.l2; return *this; }
	inline float3x3& operator-=( float3x3 const& v ) { l0 -= v.l0; l1 -= v.l1; l2 -= v.l2; return *this; }
	
	inline float3x3& operator+=( float v ) { l0 += v; l1 += v; l2 += v; return *this; }
	inline float3x3& operator-=( float v ) { l0 -= v; l1 -= v; l2 -= v; return *this; }
	inline float3x3& operator*=( float v ) { l0 *= v; l1 *= v; l2 *= v; return *this; }
	inline float3x3& operator/=( float v )
	{
		float const inv_v = 1.0f / v;
		l0 *= inv_v;
		l1 *= inv_v;
		l2 *= inv_v;
		return *this;
	}
};

inline float determinant( float3x3 const& m )
{
	return	m.a00 * ( m.a11 * m.a22 - m.a12 * m.a21 ) - 
			m.a01 * ( m.a10 * m.a22 - m.a12 * m.a20 ) +
			m.a02 * ( m.a10 * m.a21 - m.a11 * m.a20 );
}

inline float3x3 inverse( float3x3 const& m )
{
	const float det = determinant( m );

	return float3x3( float3( ( m.a11 * m.a22 - m.a12 * m.a21 ), ( m.a02 * m.a21 - m.a01 * m.a22 ), ( m.a01 * m.a12 - m.a02 * m.a11 ) ),
					 float3( ( m.a12 * m.a20 - m.a10 * m.a22 ), ( m.a00 * m.a22 - m.a02 * m.a20 ), ( m.a02 * m.a10 - m.a00 * m.a12 ) ),
					 float3( ( m.a10 * m.a21 - m.a11 * m.a20 ), ( m.a01 * m.a20 - m.a00 * m.a21 ), ( m.a00 * m.a11 - m.a01 * m.a10 ) ) ) / det;
}

inline float3x3 transpose( float3x3 const& m )
{
	return float3x3( float3( m.a00, m.a10, m.a20 ),
					 float3( m.a01, m.a11, m.a21 ),
					 float3( m.a02, m.a12, m.a22 ) );
}

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
	inline float4x4( float4x4 const& m ) :
		l0( m.l0 ), l1( m.l1 ), l2( m.l2 ), l3( m.l3 )
	{ }
	inline float4x4( float4 const& l0, float4 const& l1, float4 const& l2, float4 const& l3 ) :
		l0( l0 ), l1( l1 ), l2( l2 ), l3( l3 )
	{ }

	inline explicit float4x4( float2x2 const& m );
	inline explicit float4x4( float3x3 const& m );
	inline explicit float4x4( float4x3 const& m );

	static inline float4x4 zero( ) { return float4x4( float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ) ); }
	static inline float4x4 identity( ) { return float4x4( float4( 1.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 1.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 1.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 1.0f ) ); }
	
	inline bool operator==( float4x4 const& v ) const { return ( l0 == v.l0 ) && ( l1 == v.l1 ) && ( l2 == v.l2 ) && ( l3 == v.l3 ); }
	inline bool operator!=( float4x4 const& v ) const { return ( l0 != v.l0 ) || ( l1 != v.l1 ) || ( l2 != v.l2 ) || ( l3 != v.l3 ); }
	
	inline float4x4 operator-( ) const { return float4x4( -l0, -l1, -l2, -l3 ); }

	inline float4x4 operator+( float4x4 const& v ) const { return float4x4( l0 + v.l0, l1 + v.l1, l2 + v.l2, l3 + v.l3 ); }
	inline float4x4 operator-( float4x4 const& v ) const { return float4x4( l0 - v.l0, l1 - v.l1, l2 - v.l2, l3 - v.l3 ); }
	
	inline float4x4 operator+( float v ) const { return float4x4( l0 + v, l1 + v, l2 + v, l3 + v ); }
	inline float4x4 operator-( float v ) const { return float4x4( l0 - v, l1 - v, l2 - v, l3 - v ); }
	inline float4x4 operator*( float v ) const { return float4x4( l0 * v, l1 * v, l2 * v, l3 * v ); }
	inline float4x4 operator/( float v ) const
	{
		float const inv_v = 1.0f / v;
		return float4x4( l0 * inv_v, l1 * inv_v, l2 * inv_v, l3 * inv_v );
	}

	inline float4x4& operator+=( float4x4 const& v ) { l0 += v.l0; l1 += v.l1; l2 += v.l2; l3 += v.l3; return *this; }
	inline float4x4& operator-=( float4x4 const& v ) { l0 -= v.l0; l1 -= v.l1; l2 -= v.l2; l3 -= v.l3; return *this; }
	
	inline float4x4& operator+=( float v ) { l0 += v; l1 += v; l2 += v; l3 += v; return *this; }
	inline float4x4& operator-=( float v ) { l0 -= v; l1 -= v; l2 -= v; l3 -= v; return *this; }
	inline float4x4& operator*=( float v ) { l0 *= v; l1 *= v; l2 *= v; l3 *= v; return *this; }
	inline float4x4& operator/=( float v )
	{
		float const inv_v = 1.0f / v;
		l0 *= inv_v;
		l1 *= inv_v;
		l2 *= inv_v;
		l3 *= inv_v;
		return *this;
	}
};

inline float determinant( float4x4 const& m )
{
	const float
		A = m.a22 * m.a33 - m.a23 * m.a32,
		B = m.a21 * m.a33 - m.a23 * m.a31,
		C = m.a21 * m.a32 - m.a22 * m.a31,
		D = m.a20 * m.a33 - m.a23 * m.a30,
		E = m.a20 * m.a32 - m.a22 * m.a30,
		F = m.a20 * m.a31 - m.a21 * m.a30;

	return	m.a00 * ( A * m.a11 - B * m.a12 + C * m.a13 ) - 
			m.a01 * ( A * m.a10 - D * m.a12 + E * m.a13 ) + 
			m.a02 * ( B * m.a10 - D * m.a11 + F * m.a13 ) - 
			m.a03 * ( C * m.a10 - E * m.a11 + F * m.a12 );
}

inline float4x4 inverse( float4x4 const& m )
{
	// https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform

	float const s0 = m.a00 * m.a11 - m.a10 * m.a01;
	float const s1 = m.a00 * m.a12 - m.a10 * m.a02;
	float const s2 = m.a00 * m.a13 - m.a10 * m.a03;
	float const s3 = m.a01 * m.a12 - m.a11 * m.a02;
	float const s4 = m.a01 * m.a13 - m.a11 * m.a03;
	float const s5 = m.a02 * m.a13 - m.a12 * m.a03;

	float const c5 = m.a22 * m.a33 - m.a32 * m.a23;
	float const c4 = m.a21 * m.a33 - m.a31 * m.a23;
	float const c3 = m.a21 * m.a32 - m.a31 * m.a22;
	float const c2 = m.a20 * m.a33 - m.a30 * m.a23;
	float const c1 = m.a20 * m.a32 - m.a30 * m.a22;
	float const c0 = m.a20 * m.a31 - m.a30 * m.a21;

	float const det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;

	return float4x4( float4( m.a11 * c5 - m.a12 * c4 + m.a13 * c3,
							 -m.a01 * c5 + m.a02 * c4 - m.a03 * c3,
							 m.a31 * s5 - m.a32 * s4 + m.a33 * s3,
							 -m.a21 * s5 + m.a22 * s4 - m.a23 * s3 ),
					 float4( -m.a10 * c5 + m.a12 * c2 - m.a13 * c1,
							 m.a00 * c5 - m.a02 * c2 + m.a03 * c1,
							 -m.a30 * s5 + m.a32 * s2 - m.a33 * s1,
							 m.a20 * s5 - m.a22 * s2 + m.a23 * s1 ),
					 float4( m.a10 * c4 - m.a11 * c2 + m.a13 * c0,
							 -m.a00 * c4 + m.a01 * c2 - m.a03 * c0,
							 m.a30 * s4 - m.a31 * s2 + m.a33 * s0,
							 -m.a20 * s4 + m.a21 * s2 - m.a23 * s0 ),
					 float4( -m.a10 * c3 + m.a11 * c1 - m.a12 * c0,
							 m.a00 * c3 - m.a01 * c1 + m.a02 * c0,
							 -m.a30 * s3 + m.a31 * s1 - m.a32 * s0,
							 m.a20 * s3 - m.a21 * s1 + m.a22 * s0 ) ) / det;
}

inline float4x4 transpose( float4x4 const& m )
{
	return float4x4( float4( m.a00, m.a10, m.a20, m.a30 ),
					 float4( m.a01, m.a11, m.a21, m.a31 ),
					 float4( m.a02, m.a12, m.a22, m.a32 ),
					 float4( m.a03, m.a13, m.a23, m.a33 ) );
}

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
	inline float4x3( float4x3 const& m ) :
		l0( m.l0 ), l1( m.l1 ), l2( m.l2 )
	{ }
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

	static inline float4x3 zero( ) { return float4x3( float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 0.0f, 0.0f ) ); }
	static inline float4x3 identity( ) { return float4x3( float4( 1.0f, 0.0f, 0.0f, 0.0f ), float4( 0.0f, 1.0f, 0.0f, 0.0f ), float4( 0.0f, 0.0f, 1.0f, 0.0f ) ); }
	
	inline bool operator==( float4x3 const& v ) const { return ( l0 == v.l0 ) && ( l1 == v.l1 ) && ( l2 == v.l2 ); }
	inline bool operator!=( float4x3 const& v ) const { return ( l0 != v.l0 ) || ( l1 != v.l1 ) || ( l2 != v.l2 ); }
	
	inline float4x3 operator-( ) const { return float4x3( -l0, -l1, -l2 ); }

	inline float4x3 operator+( float4x3 const& v ) const { return float4x3( l0 + v.l0, l1 + v.l1, l2 + v.l2 ); }
	inline float4x3 operator-( float4x3 const& v ) const { return float4x3( l0 - v.l0, l1 - v.l1, l2 - v.l2 ); }
	
	inline float4x3 operator+( float v ) const { return float4x3( l0 + v, l1 + v, l2 + v ); }
	inline float4x3 operator-( float v ) const { return float4x3( l0 - v, l1 - v, l2 - v ); }
	inline float4x3 operator*( float v ) const { return float4x3( l0 * v, l1 * v, l2 * v ); }
	inline float4x3 operator/( float v ) const
	{
		float const inv_v = 1.0f / v;
		return float4x3( l0 * inv_v, l1 * inv_v, l2 * inv_v );
	}

	inline float4x3& operator+=( float4x3 const& v ) { l0 += v.l0; l1 += v.l1; l2 += v.l2; return *this; }
	inline float4x3& operator-=( float4x3 const& v ) { l0 -= v.l0; l1 -= v.l1; l2 -= v.l2; return *this; }
	
	inline float4x3& operator+=( float v ) { l0 += v; l1 += v; l2 += v; return *this; }
	inline float4x3& operator-=( float v ) { l0 -= v; l1 -= v; l2 -= v; return *this; }
	inline float4x3& operator*=( float v ) { l0 *= v; l1 *= v; l2 *= v; return *this; }
	inline float4x3& operator/=( float v )
	{
		float const inv_v = 1.0f / v;
		l0 *= inv_v;
		l1 *= inv_v;
		l2 *= inv_v;
		return *this;
	}
};

inline float determinant( float4x3 const& m )
{
	return	m.a00 * ( m.a22 * m.a11 - m.a21 * m.a12 ) - 
			m.a01 * ( m.a22 * m.a10 - m.a20 * m.a12 ) + 
			m.a02 * ( m.a21 * m.a10 - m.a20 * m.a11 );
}

inline float4x3 inverse( float4x3 const& m )
{
	// https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform

	float const s0 = m.a00 * m.a11 - m.a10 * m.a01;
	float const s1 = m.a00 * m.a12 - m.a10 * m.a02;
	float const s2 = m.a00 * m.a13 - m.a10 * m.a03;
	float const s3 = m.a01 * m.a12 - m.a11 * m.a02;
	float const s4 = m.a01 * m.a13 - m.a11 * m.a03;
	float const s5 = m.a02 * m.a13 - m.a12 * m.a03;

	float const det = s0 * m.a22 - s1 * m.a21 + s3 * m.a20;

	return float4x3( float4( m.a11 * m.a22 - m.a12 * m.a21,
							 -m.a01 * m.a22 + m.a02 * m.a21,
							 s3,
							 -m.a21 * s5 + m.a22 * s4 - m.a23 * s3 ),
					 float4( -m.a10 * m.a22 + m.a12 * m.a20,
							 m.a00 * m.a22 - m.a02 * m.a20,
							 s1,
							 m.a20 * s5 - m.a22 * s2 + m.a23 * s1 ),
					 float4( m.a10 * m.a21 - m.a11 * m.a20,
							 -m.a00 * m.a21 + m.a01 * m.a20,
							 s0,
							 -m.a20 * s4 + m.a21 * s2 - m.a23 * s0 ) ) / det;
}


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

inline float2 mul( float2x2 const& m, float2 const& v ) { return float2( dot( m.l0, v ), dot( m.l1, v ) ); }
inline float3 mul( float3x3 const& m, float3 const& v ) { return float3( dot( m.l0, v ), dot( m.l1, v ), dot( m.l2, v ) ); }
inline float4 mul( float4x4 const& m, float4 const& v ) { return float4( dot( m.l0, v ), dot( m.l1, v ), dot( m.l2, v ), dot( m.l3, v ) ); }

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