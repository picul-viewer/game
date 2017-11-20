#ifndef __core_math_sse_h_included_
#define __core_math_sse_h_included_

#include <core/types.h>
#include <core/macros.h>
#include <xmmintrin.h>

namespace math {

namespace sse {

// Assume everything aligned

struct matrix
{
	__m128 data[4];

	inline operator __m128*( ) { return data; }
	inline __m128& operator[]( u32 i ) { ASSERT( i < 4 ); return data[i]; }
	inline __m128 operator[]( u32 i ) const { ASSERT( i < 4 ); return data[i]; }
};

inline void matrix_transpose( matrix& m )
{
	_MM_TRANSPOSE4_PS( m[0], m[1], m[2], m[3] );
}

inline void matrix_multiply( matrix const& left, matrix const& right, matrix& result )
{
	const __m128 l00 = _mm_shuffle_ps( left[0], left[0], 0x00 );
	const __m128 l01 = _mm_shuffle_ps( left[0], left[0], 0x55 );
	const __m128 l02 = _mm_shuffle_ps( left[0], left[0], 0xAA );
	const __m128 l03 = _mm_shuffle_ps( left[0], left[0], 0xFF );

	result[0] = _mm_mul_ps( l00, right[0] );
	result[0] = _mm_add_ps( result[0], _mm_mul_ps( l01, right[1] ) );
	result[0] = _mm_add_ps( result[0], _mm_mul_ps( l02, right[2] ) );
	result[0] = _mm_add_ps( result[0], _mm_mul_ps( l03, right[3] ) );
	
	const __m128 l10 = _mm_shuffle_ps( left[1], left[1], 0x00 );
	const __m128 l11 = _mm_shuffle_ps( left[1], left[1], 0x55 );
	const __m128 l12 = _mm_shuffle_ps( left[1], left[1], 0xAA );
	const __m128 l13 = _mm_shuffle_ps( left[1], left[1], 0xFF );

	result[1] = _mm_mul_ps( l10, right[0] );
	result[1] = _mm_add_ps( result[1], _mm_mul_ps( l11, right[1] ) );
	result[1] = _mm_add_ps( result[1], _mm_mul_ps( l12, right[2] ) );
	result[1] = _mm_add_ps( result[1], _mm_mul_ps( l13, right[3] ) );
	
	const __m128 l20 = _mm_shuffle_ps( left[2], left[2], 0x00 );
	const __m128 l21 = _mm_shuffle_ps( left[2], left[2], 0x55 );
	const __m128 l22 = _mm_shuffle_ps( left[2], left[2], 0xAA );
	const __m128 l23 = _mm_shuffle_ps( left[2], left[2], 0xFF );

	result[2] = _mm_mul_ps( l20, right[0] );
	result[2] = _mm_add_ps( result[2], _mm_mul_ps( l21, right[1] ) );
	result[2] = _mm_add_ps( result[2], _mm_mul_ps( l22, right[2] ) );
	result[2] = _mm_add_ps( result[2], _mm_mul_ps( l23, right[3] ) );
	
	const __m128 l30 = _mm_shuffle_ps( left[3], left[3], 0x00 );
	const __m128 l31 = _mm_shuffle_ps( left[3], left[3], 0x55 );
	const __m128 l32 = _mm_shuffle_ps( left[3], left[3], 0xAA );
	const __m128 l33 = _mm_shuffle_ps( left[3], left[3], 0xFF );

	result[3] = _mm_mul_ps( l30, right[0] );
	result[3] = _mm_add_ps( result[3], _mm_mul_ps( l31, right[1] ) );
	result[3] = _mm_add_ps( result[3], _mm_mul_ps( l32, right[2] ) );
	result[3] = _mm_add_ps( result[3], _mm_mul_ps( l33, right[3] ) );
}

inline void matrix_multiply( float const* left, float const* right, float* result )
{
	const __m128 r0 = _mm_load_ps( result + 0 );
	const __m128 r1 = _mm_load_ps( result + 4 );
	const __m128 r2 = _mm_load_ps( result + 8 );
	const __m128 r3 = _mm_load_ps( result + 12 );
	
	__m128 res;

	const __m128 l00 = _mm_set1_ps( left[0] );
	const __m128 l01 = _mm_set1_ps( left[1] );
	const __m128 l02 = _mm_set1_ps( left[2] );
	const __m128 l03 = _mm_set1_ps( left[3] );

	res = _mm_mul_ps( l00, r0 );
	res = _mm_add_ps( res, _mm_mul_ps( l01, r1 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l02, r2 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l03, r3 ) );

	_mm_store_ps( result + 0, res );
	
	const __m128 l10 = _mm_set1_ps( left[4] );
	const __m128 l11 = _mm_set1_ps( left[5] );
	const __m128 l12 = _mm_set1_ps( left[6] );
	const __m128 l13 = _mm_set1_ps( left[7] );

	res = _mm_mul_ps( l10, r0 );
	res = _mm_add_ps( res, _mm_mul_ps( l11, r1 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l12, r2 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l13, r3 ) );
	
	_mm_store_ps( result + 4, res );
	
	const __m128 l20 = _mm_set1_ps( left[8] );
	const __m128 l21 = _mm_set1_ps( left[9] );
	const __m128 l22 = _mm_set1_ps( left[10] );
	const __m128 l23 = _mm_set1_ps( left[11] );

	res = _mm_mul_ps( l20, r0 );
	res = _mm_add_ps( res, _mm_mul_ps( l21, r1 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l22, r2 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l23, r3 ) );
	
	_mm_store_ps( result + 8, res );
	
	const __m128 l30 = _mm_set1_ps( left[12] );
	const __m128 l31 = _mm_set1_ps( left[13] );
	const __m128 l32 = _mm_set1_ps( left[14] );
	const __m128 l33 = _mm_set1_ps( left[15] );

	res = _mm_mul_ps( l30, r0 );
	res = _mm_add_ps( res, _mm_mul_ps( l31, r1 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l32, r2 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l33, r3 ) );

	_mm_store_ps( result + 12, res );
}

// Store result in v
inline void modify_by_transform( float* v, float const* m )
{
	__m128 l0 = _mm_load_ps( m + 0 );
	__m128 l1 = _mm_load_ps( m + 4 );
	__m128 l2 = _mm_load_ps( m + 8 );
	__m128 l3 = _mm_load_ps( m + 12 );

	_MM_TRANSPOSE4_PS( l0, l1, l2, l3 );

	__m128 vec = _mm_load_ps( v );

	__m128 vec0 = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 vec1 = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 vec2 = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
	__m128 vec3 = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 3, 3, 3, 3 ) );

	__m128 res = _mm_add_ps(
		_mm_add_ps( _mm_mul_ps( l0, vec0 ), _mm_mul_ps( l1, vec1 ) ),
		_mm_add_ps( _mm_mul_ps( l2, vec2 ), _mm_mul_ps( l3, vec3 ) ) );

	_mm_store_ps( v, res );
}

// Need four input components in 'v', but assume that w=1
// Store result in v, only x,y,z are relevant
inline void modify_position_by_transform( float* v, float const* m )
{
	__m128 l0 = _mm_load_ps( m + 0 );
	__m128 l1 = _mm_load_ps( m + 4 );
	__m128 l2 = _mm_load_ps( m + 8 );
	__m128 l3 = _mm_load_ps( m + 12 );

	_MM_TRANSPOSE4_PS( l0, l1, l2, l3 );

	__m128 vec = _mm_load_ps( v );

	__m128 vec0 = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 vec1 = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 vec2 = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

	__m128 res = _mm_add_ps(
		_mm_add_ps( _mm_mul_ps( l0, vec0 ), _mm_mul_ps( l1, vec1 ) ),
		_mm_add_ps( _mm_mul_ps( l2, vec2 ), l3 ) );

	_mm_store_ps( v, res );
}

// Only 12 elements are necessary in 'left' and 'right'
// Only 12 elements are modifying in 'result'
inline void combine_transforms( float const* left, float const* right, float* result )
{
	const __m128 r0 = _mm_load_ps( result + 0 );
	const __m128 r1 = _mm_load_ps( result + 4 );
	const __m128 r2 = _mm_load_ps( result + 8 );
	
	__m128 res;

	const __m128 l00 = _mm_set1_ps( left[0] );
	const __m128 l01 = _mm_set1_ps( left[1] );
	const __m128 l02 = _mm_set1_ps( left[2] );
	const __m128 l03 = _mm_set1_ps( left[3] ); 

	res = _mm_add_ps( l03, _mm_mul_ps( l00, r0 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l01, r1 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l02, r2 ) );

	_mm_store_ps( result + 0, res );
	
	const __m128 l10 = _mm_set1_ps( left[4] );
	const __m128 l11 = _mm_set1_ps( left[5] );
	const __m128 l12 = _mm_set1_ps( left[6] );
	const __m128 l13 = _mm_set1_ps( left[7] );

	res = _mm_add_ps( l13, _mm_mul_ps( l10, r0 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l11, r1 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l12, r2 ) );
	
	_mm_store_ps( result + 4, res );
	
	const __m128 l20 = _mm_set1_ps( left[8] );
	const __m128 l21 = _mm_set1_ps( left[9] );
	const __m128 l22 = _mm_set1_ps( left[10] );
	const __m128 l23 = _mm_set1_ps( left[11] );

	res = _mm_add_ps( l23, _mm_mul_ps( l20, r0 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l21, r1 ) );
	res = _mm_add_ps( res, _mm_mul_ps( l22, r2 ) );
	
	_mm_store_ps( result + 8, res );
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

inline void matrix_inverse( matrix const& m, matrix& result )
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
}

} // namespace sse

} // namespace math

#endif // #ifndef __core_math_sse_h_included_