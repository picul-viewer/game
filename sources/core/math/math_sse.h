#ifndef GUARD_CORE_MATH_SSE_H_INCLUDED
#define GUARD_CORE_MATH_SSE_H_INCLUDED

#include <types.h>
#include <macros.h>
#include "vector.h"
#include "matrix.h"
#include <nmmintrin.h>

namespace math {

namespace sse {

// Assume everything aligned

mem_align(16)
struct vector
{
	__m128 data;
	
	vector( ) = default;

	inline vector( __m128 data ) : data( data ) { }
	inline vector( math::float4 const& data )
	{
		loadu( data.data );
	}
	
	inline void load( float const* data )
	{
		ASSERT( aligned( data, 16 ) );
		this->data = _mm_load_ps( data );
	}

	inline void loadu( float const* data )
	{
		this->data = _mm_loadu_ps( data );
	}

	inline operator __m128( ) const { return data; }
	inline operator math::float4( ) const
	{
		mem_align(16)
		math::float4 res;

		_mm_store_ps( res.data, data );

		return res;
	}
};

static inline vector operator+( vector a, vector b ) { return _mm_add_ps( a.data, b.data ); }
static inline vector operator-( vector a, vector b ) { return _mm_sub_ps( a.data, b.data ); }
static inline vector operator*( vector a, vector b ) { return _mm_mul_ps( a.data, b.data ); }
static inline vector operator/( vector a, vector b ) { return _mm_div_ps( a.data, b.data ); }

struct matrix;

mem_align(16)
struct matrix3
{
	__m128 data[3];

	matrix3( ) = default;

	inline matrix3( __m128* data ) : data{ data[0], data[1], data[2] } { }
	inline matrix3( math::float4x3 const& data )
	{
		loadu( data.data );
	}
	
	inline explicit matrix3( matrix const& m );

	inline void load( float const* data )
	{
		ASSERT( aligned( data, 16 ) );
		this->data[0] = _mm_load_ps( data + 0 );
		this->data[1] = _mm_load_ps( data + 4 );
		this->data[2] = _mm_load_ps( data + 8 );
	}
	
	inline void loadu( float const* data )
	{
		this->data[0] = _mm_loadu_ps( data + 0 );
		this->data[1] = _mm_loadu_ps( data + 4 );
		this->data[2] = _mm_loadu_ps( data + 8 );
	}

	inline operator __m128*( ) { return data; }
	inline operator math::float4x3( ) const
	{
		mem_align(16)
		math::float4x3 res;

		_mm_store_ps( res.l0.data, data[0] );
		_mm_store_ps( res.l1.data, data[1] );
		_mm_store_ps( res.l2.data, data[2] );

		return res;
	}

	inline __m128& operator[]( u32 const i ) { ASSERT_CMP( i, <, 3 ); return data[i]; }
	inline __m128 operator[]( u32 const i ) const { ASSERT_CMP( i, <, 3 ); return data[i]; }
};

mem_align(16)
struct matrix
{
	__m128 data[4];

	matrix( ) = default;

	inline matrix( __m128* data ) : data{ data[0], data[1], data[2], data[3] } { }
	inline matrix( math::float4x4 const& data )
	{
		loadu( data.data );
	}

	inline void load( float const* data )
	{
		ASSERT( aligned( data, 16 ) );
		this->data[0] = _mm_load_ps( data + 0 );
		this->data[1] = _mm_load_ps( data + 4 );
		this->data[2] = _mm_load_ps( data + 8 );
		this->data[3] = _mm_load_ps( data + 12 );
	}
	
	inline void loadu( float const* data )
	{
		this->data[0] = _mm_loadu_ps( data + 0 );
		this->data[1] = _mm_loadu_ps( data + 4 );
		this->data[2] = _mm_loadu_ps( data + 8 );
		this->data[3] = _mm_loadu_ps( data + 12 );
	}
	
	inline explicit matrix( matrix3 const& data )
	{
		ASSERT( aligned( &data, 16 ) );
		this->data[0] = data[0];
		this->data[1] = data[1];
		this->data[2] = data[2];
		this->data[3] = _mm_setr_ps( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	inline operator __m128*( ) { return data; }
	inline operator math::float4x4( ) const
	{
		mem_align(16)
		math::float4x4 res;

		_mm_store_ps( res.l0.data, data[0] );
		_mm_store_ps( res.l1.data, data[1] );
		_mm_store_ps( res.l2.data, data[2] );
		_mm_store_ps( res.l3.data, data[3] );

		return res;
	}

	inline __m128& operator[]( u32 const i ) { ASSERT_CMP( i, <, 4 ); return data[i]; }
	inline __m128 operator[]( u32 const i ) const { ASSERT_CMP( i, <, 4 ); return data[i]; }
};

matrix3::matrix3( matrix const& m ) : data{ m.data[0], m.data[1], m.data[2] } { }

inline void matrix_transpose( matrix& m )
{
	_MM_TRANSPOSE4_PS( m[0], m[1], m[2], m[3] );
}

inline matrix matrix_multiply( matrix const& left, matrix const& right )
{
	matrix result;

	__m128 const l00 = _mm_shuffle_ps( left[0], left[0], 0x00 );
	__m128 const l01 = _mm_shuffle_ps( left[0], left[0], 0x55 );
	__m128 const l02 = _mm_shuffle_ps( left[0], left[0], 0xAA );
	__m128 const l03 = _mm_shuffle_ps( left[0], left[0], 0xFF );

	result[0] = _mm_add_ps( _mm_add_ps( _mm_mul_ps( l00, right[0] ),
										_mm_mul_ps( l01, right[1] ) ),
							_mm_add_ps( _mm_mul_ps( l02, right[2] ),
										_mm_mul_ps( l03, right[3] ) ) );
	
	__m128 const l10 = _mm_shuffle_ps( left[1], left[1], 0x00 );
	__m128 const l11 = _mm_shuffle_ps( left[1], left[1], 0x55 );
	__m128 const l12 = _mm_shuffle_ps( left[1], left[1], 0xAA );
	__m128 const l13 = _mm_shuffle_ps( left[1], left[1], 0xFF );
	
	result[1] = _mm_add_ps( _mm_add_ps( _mm_mul_ps( l10, right[0] ),
										_mm_mul_ps( l11, right[1] ) ),
							_mm_add_ps( _mm_mul_ps( l12, right[2] ),
										_mm_mul_ps( l13, right[3] ) ) );
	
	__m128 const l20 = _mm_shuffle_ps( left[2], left[2], 0x00 );
	__m128 const l21 = _mm_shuffle_ps( left[2], left[2], 0x55 );
	__m128 const l22 = _mm_shuffle_ps( left[2], left[2], 0xAA );
	__m128 const l23 = _mm_shuffle_ps( left[2], left[2], 0xFF );
	
	result[2] = _mm_add_ps( _mm_add_ps( _mm_mul_ps( l20, right[0] ),
										_mm_mul_ps( l21, right[1] ) ),
							_mm_add_ps( _mm_mul_ps( l22, right[2] ),
										_mm_mul_ps( l23, right[3] ) ) );
	
	__m128 const l30 = _mm_shuffle_ps( left[3], left[3], 0x00 );
	__m128 const l31 = _mm_shuffle_ps( left[3], left[3], 0x55 );
	__m128 const l32 = _mm_shuffle_ps( left[3], left[3], 0xAA );
	__m128 const l33 = _mm_shuffle_ps( left[3], left[3], 0xFF );
	
	result[3] = _mm_add_ps( _mm_add_ps( _mm_mul_ps( l30, right[0] ),
										_mm_mul_ps( l31, right[1] ) ),
							_mm_add_ps( _mm_mul_ps( l32, right[2] ),
										_mm_mul_ps( l33, right[3] ) ) );
	
	return result;
}

inline vector modify_by_transform( vector const& v, matrix const& m )
{
	__m128 ml0 = _mm_mul_ps( m[0], v );
	__m128 ml1 = _mm_mul_ps( m[1], v );
	__m128 ml2 = _mm_mul_ps( m[2], v );
	__m128 ml3 = _mm_mul_ps( m[3], v );

	_MM_TRANSPOSE4_PS( ml0, ml1, ml2, ml3 );

	__m128 res = _mm_add_ps( _mm_add_ps( ml0, ml1 ), _mm_add_ps( ml2, ml3 ) );

	return res;
}

// Need four input components in 'v', but assume that w=1
// Only x,y,z are relevant in result
inline vector modify_position_by_transform( vector const& v, matrix const& m )
{
	__m128 l0 = m[0];
	__m128 l1 = m[1];
	__m128 l2 = m[2];
	__m128 l3 = m[3];

	_MM_TRANSPOSE4_PS( l0, l1, l2, l3 );

	__m128 vec0 = _mm_shuffle_ps( v, v, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 vec1 = _mm_shuffle_ps( v, v, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 vec2 = _mm_shuffle_ps( v, v, _MM_SHUFFLE( 2, 2, 2, 2 ) );

	__m128 res = _mm_add_ps(
		_mm_add_ps( _mm_mul_ps( l0, vec0 ), _mm_mul_ps( l1, vec1 ) ),
		_mm_add_ps( _mm_mul_ps( l2, vec2 ), l3 ) );

	return res;
}


inline matrix combine_transforms( matrix const& m1, matrix const& m2 )
{
	return matrix_multiply( m2, m1 );
}

// Only 12 elements are necessary in 'left' and 'right'
// Only 12 elements are modifying in 'result'
inline matrix3 combine_transforms( matrix3 const& left, matrix3 const& right )
{
	matrix3 result;

	__m128 const last_row_mask = _mm_castsi128_ps( _mm_setr_epi32( 0x0, 0x0, 0x0, 0xFFFFFFFF ) );

	__m128 const l00 = _mm_shuffle_ps( right[0], right[0], 0x00 );
	__m128 const l01 = _mm_shuffle_ps( right[0], right[0], 0x55 );
	__m128 const l02 = _mm_shuffle_ps( right[0], right[0], 0xAA );
	__m128 const l03 = _mm_shuffle_ps( right[0], right[0], 0xFF );

	result[0] = _mm_add_ps( _mm_add_ps( _mm_mul_ps( l00, left[0] ),
										_mm_mul_ps( l01, left[1] ) ),
							_mm_add_ps( _mm_mul_ps( l02, left[2] ),
										_mm_and_ps( l03, last_row_mask ) ) );
	
	__m128 const l10 = _mm_shuffle_ps( right[1], right[1], 0x00 );
	__m128 const l11 = _mm_shuffle_ps( right[1], right[1], 0x55 );
	__m128 const l12 = _mm_shuffle_ps( right[1], right[1], 0xAA );
	__m128 const l13 = _mm_shuffle_ps( right[1], right[1], 0xFF );
	
	result[1] = _mm_add_ps( _mm_add_ps( _mm_mul_ps( l10, left[0] ),
										_mm_mul_ps( l11, left[1] ) ),
							_mm_add_ps( _mm_mul_ps( l12, left[2] ),
										_mm_and_ps( l13, last_row_mask ) ) );
	
	__m128 const l20 = _mm_shuffle_ps( right[2], right[2], 0x00 );
	__m128 const l21 = _mm_shuffle_ps( right[2], right[2], 0x55 );
	__m128 const l22 = _mm_shuffle_ps( right[2], right[2], 0xAA );
	__m128 const l23 = _mm_shuffle_ps( right[2], right[2], 0xFF );
	
	result[2] = _mm_add_ps( _mm_add_ps( _mm_mul_ps( l20, left[0] ),
										_mm_mul_ps( l21, left[1] ) ),
							_mm_add_ps( _mm_mul_ps( l22, left[2] ),
										_mm_and_ps( l23, last_row_mask ) ) );
	
	return result;
}

inline matrix combine_transform_and_perspective_projection( matrix const& view, matrix const& perspective_projection )
{
	/*
	return combine_transforms( view, perspective_projection );
	*/

	__m128 const p00 = _mm_shuffle_ps( perspective_projection[0], perspective_projection[0], 0x00 );
	__m128 const p11 = _mm_shuffle_ps( perspective_projection[1], perspective_projection[1], 0x55 );
	__m128 const p22 = _mm_shuffle_ps( perspective_projection[2], perspective_projection[2], 0xAA );
	__m128 const p23 = _mm_shuffle_ps( perspective_projection[2], perspective_projection[2], 0xFF );

	matrix result;

	result[0] = _mm_mul_ps( view[0], p00 );
	result[1] = _mm_mul_ps( view[1], p11 );
	result[2] = _mm_add_ps( _mm_mul_ps( view[2], p22 ), _mm_mul_ps( view[3], p23 ) );
	result[3] = view[2];

	return result;
}

inline float matrix_determinant( matrix const& m )
{
	// Layout (LtoH): ( s0 s1 s2 s3 ) ( c5 c4 c3 c2 ) ( s4 s5 c1 c0 )
	__m128 sub_det[3];
	sub_det[0]		= _mm_mul_ps( _mm_shuffle_ps( m[0], m[0], _MM_SHUFFLE( 1, 0, 0, 0 ) ), _mm_shuffle_ps( m[1], m[1], _MM_SHUFFLE( 2, 3, 2, 1 ) ) );
	sub_det[1]		= _mm_mul_ps( _mm_shuffle_ps( m[2], m[2], _MM_SHUFFLE( 0, 1, 1, 2 ) ), _mm_shuffle_ps( m[3], m[3], _MM_SHUFFLE( 3, 2, 3, 3 ) ) );
	sub_det[2]		= _mm_mul_ps( _mm_shuffle_ps( m[0], m[2], _MM_SHUFFLE( 0, 0, 2, 1 ) ), _mm_shuffle_ps( m[1], m[3], _MM_SHUFFLE( 1, 2, 3, 3 ) ) );

	sub_det[0]		= _mm_sub_ps( sub_det[0], _mm_mul_ps( _mm_shuffle_ps( m[1], m[1], _MM_SHUFFLE( 1, 0, 0, 0 ) ), _mm_shuffle_ps( m[0], m[0], _MM_SHUFFLE( 2, 3, 2, 1 ) ) ) );
	sub_det[1]		= _mm_sub_ps( sub_det[1], _mm_mul_ps( _mm_shuffle_ps( m[3], m[3], _MM_SHUFFLE( 0, 1, 1, 2 ) ), _mm_shuffle_ps( m[2], m[2], _MM_SHUFFLE( 3, 2, 3, 3 ) ) ) );
	sub_det[2]		= _mm_sub_ps( sub_det[2], _mm_mul_ps( _mm_shuffle_ps( m[1], m[3], _MM_SHUFFLE( 0, 0, 2, 1 ) ), _mm_shuffle_ps( m[0], m[2], _MM_SHUFFLE( 1, 2, 3, 3 ) ) ) );

	__m128 det_mul[2];
	det_mul[0]		= _mm_mul_ps( sub_det[0], sub_det[1] );
	det_mul[1]		= _mm_mul_ps( sub_det[2], _mm_shuffle_ps( sub_det[2], _mm_setzero_ps( ), _MM_SHUFFLE( 0, 0, 3, 2 ) ) );

	det_mul[0]		= _mm_sub_ps( det_mul[0], det_mul[1] );

	det_mul[0]		= _mm_hadd_ps( det_mul[0], det_mul[0] );
	det_mul[0]		= _mm_hadd_ps( det_mul[0], det_mul[0] );

	float det;
	_mm_store1_ps	( &det, det_mul[0] );

	return det;
}

inline matrix matrix_inverse( matrix const& m )
{
	// Layout (LtoH): ( s0 s1 s2 s3 ) ( c5 c4 c3 c2 ) ( s4 s5 c1 c0 )
	__m128 sub_det[3];
	sub_det[0]		= _mm_mul_ps( _mm_shuffle_ps( m[0], m[0], _MM_SHUFFLE( 1, 0, 0, 0 ) ), _mm_shuffle_ps( m[1], m[1], _MM_SHUFFLE( 2, 3, 2, 1 ) ) );
	sub_det[1]		= _mm_mul_ps( _mm_shuffle_ps( m[2], m[2], _MM_SHUFFLE( 0, 1, 1, 2 ) ), _mm_shuffle_ps( m[3], m[3], _MM_SHUFFLE( 3, 2, 3, 3 ) ) );
	sub_det[2]		= _mm_mul_ps( _mm_shuffle_ps( m[0], m[2], _MM_SHUFFLE( 0, 0, 2, 1 ) ), _mm_shuffle_ps( m[1], m[3], _MM_SHUFFLE( 1, 2, 3, 3 ) ) );

	sub_det[0]		= _mm_sub_ps( sub_det[0], _mm_mul_ps( _mm_shuffle_ps( m[1], m[1], _MM_SHUFFLE( 1, 0, 0, 0 ) ), _mm_shuffle_ps( m[0], m[0], _MM_SHUFFLE( 2, 3, 2, 1 ) ) ) );
	sub_det[1]		= _mm_sub_ps( sub_det[1], _mm_mul_ps( _mm_shuffle_ps( m[3], m[3], _MM_SHUFFLE( 0, 1, 1, 2 ) ), _mm_shuffle_ps( m[2], m[2], _MM_SHUFFLE( 3, 2, 3, 3 ) ) ) );
	sub_det[2]		= _mm_sub_ps( sub_det[2], _mm_mul_ps( _mm_shuffle_ps( m[1], m[3], _MM_SHUFFLE( 0, 0, 2, 1 ) ), _mm_shuffle_ps( m[0], m[2], _MM_SHUFFLE( 1, 2, 3, 3 ) ) ) );

	__m128 det_mul[2];
	det_mul[0]		= _mm_mul_ps( sub_det[0], sub_det[1] );
	det_mul[1]		= _mm_mul_ps( sub_det[2], _mm_shuffle_ps( sub_det[2], _mm_setzero_ps( ), _MM_SHUFFLE( 0, 0, 3, 2 ) ) );

	det_mul[0]		= _mm_sub_ps( det_mul[0], det_mul[1] );
	
	det_mul[0]		= _mm_hadd_ps( det_mul[0], det_mul[0] );
	det_mul[0]		= _mm_hadd_ps( det_mul[0], det_mul[0] );

	__m128 inv_det	= _mm_shuffle_ps( det_mul[0], det_mul[0], 0x00 );
	inv_det			= _mm_rcp_ps( inv_det );

	__m128 m_transp1[4];
	m_transp1[0]	= _mm_shuffle_ps( m[0], m[1], _MM_SHUFFLE( 1, 0, 1, 0 ) );
	m_transp1[1]	= _mm_shuffle_ps( m[0], m[1], _MM_SHUFFLE( 3, 2, 3, 2 ) );
	m_transp1[2]	= _mm_shuffle_ps( m[2], m[3], _MM_SHUFFLE( 1, 0, 1, 0 ) );
	m_transp1[3]	= _mm_shuffle_ps( m[2], m[3], _MM_SHUFFLE( 3, 2, 3, 2 ) );

	__m128 m_transp[4];
	m_transp[0]		= _mm_shuffle_ps( m_transp1[0], m_transp1[2], _MM_SHUFFLE( 0, 2, 0, 2 ) );
	m_transp[1]		= _mm_shuffle_ps( m_transp1[0], m_transp1[2], _MM_SHUFFLE( 1, 3, 1, 3 ) );
	m_transp[2]		= _mm_shuffle_ps( m_transp1[1], m_transp1[3], _MM_SHUFFLE( 0, 2, 0, 2 ) );
	m_transp[3]		= _mm_shuffle_ps( m_transp1[1], m_transp1[3], _MM_SHUFFLE( 1, 3, 1, 3 ) );

	__m128 sub_dets[6];
	sub_dets[0]		= _mm_shuffle_ps( sub_det[2], sub_det[0], _MM_SHUFFLE( 0, 0, 3, 3 ) );
	sub_dets[1]		= _mm_shuffle_ps( sub_det[2], sub_det[0], _MM_SHUFFLE( 1, 1, 2, 2 ) );
	sub_dets[2]		= _mm_shuffle_ps( sub_det[1], sub_det[0], _MM_SHUFFLE( 2, 2, 3, 3 ) );
	sub_dets[3]		= _mm_shuffle_ps( sub_det[1], sub_det[0], _MM_SHUFFLE( 3, 3, 2, 2 ) );
	sub_dets[4]		= _mm_shuffle_ps( sub_det[1], sub_det[2], _MM_SHUFFLE( 0, 0, 1, 1 ) );
	sub_dets[5]		= _mm_shuffle_ps( sub_det[1], sub_det[2], _MM_SHUFFLE( 1, 1, 0, 0 ) );

	__m128 neg_pattern;
	neg_pattern		= _mm_castsi128_ps( _mm_setr_epi32( 0x00000000, 0x80000000, 0x00000000, 0x80000000 ) );

	matrix result;

	result[0]		= _mm_mul_ps( m_transp[1], sub_dets[5] );
	result[0]		= _mm_sub_ps( result[0], _mm_mul_ps( m_transp[2], sub_dets[4] ) );
	result[0]		= _mm_add_ps( result[0], _mm_mul_ps( m_transp[3], sub_dets[3] ) );
	result[0]		= _mm_xor_ps( result[0], neg_pattern );
	
	result[1]		= _mm_mul_ps( m_transp[2], sub_dets[2] );
	result[1]		= _mm_sub_ps( result[1], _mm_mul_ps( m_transp[0], sub_dets[5] ) );
	result[1]		= _mm_sub_ps( result[1], _mm_mul_ps( m_transp[3], sub_dets[1] ) );
	result[1]		= _mm_xor_ps( result[1], neg_pattern );
	
	result[2]		= _mm_mul_ps( m_transp[0], sub_dets[4] );
	result[2]		= _mm_sub_ps( result[2], _mm_mul_ps( m_transp[1], sub_dets[2] ) );
	result[2]		= _mm_add_ps( result[2], _mm_mul_ps( m_transp[3], sub_dets[0] ) );
	result[2]		= _mm_xor_ps( result[2], neg_pattern );
	
	result[3]		= _mm_mul_ps( m_transp[1], sub_dets[1] );
	result[3]		= _mm_sub_ps( result[3], _mm_mul_ps( m_transp[0], sub_dets[3] ) );
	result[3]		= _mm_add_ps( result[3], _mm_mul_ps( m_transp[2], sub_dets[0] ) );
	result[3]		= _mm_xor_ps( result[3], neg_pattern );

	result[0]		= _mm_mul_ps( result[0], inv_det );
	result[1]		= _mm_mul_ps( result[1], inv_det );
	result[2]		= _mm_mul_ps( result[2], inv_det );
	result[3]		= _mm_mul_ps( result[3], inv_det );

	return result;
}

inline matrix matrix_transformation_inverse( matrix const& m )
{
	matrix result;

	__m128 const a30 = _mm_shuffle_ps( m[3], m[3], 0x00 );
	__m128 const a31 = _mm_shuffle_ps( m[3], m[3], 0x55 );
	__m128 const a32 = _mm_shuffle_ps( m[3], m[3], 0xAA );

	__m128 const negate_mask = _mm_castsi128_ps( _mm_setr_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );

	result[0] = m[0];
	result[1] = m[1];
	result[2] = m[2];
	result[3] = _mm_sub_ps( _mm_xor_ps( _mm_mul_ps( a30, m[0] ), negate_mask ),
							_mm_add_ps( _mm_mul_ps( a31, m[1] ),
										_mm_mul_ps( a32, m[2] ) ) );

	matrix_transpose( result );

	result[3] = _mm_setr_ps( 0.0f, 0.0f, 0.0f, 1.0f );

	return result;
}

} // namespace sse

} // namespace math

#endif // #ifndef GUARD_CORE_MATH_SSE_H_INCLUDED