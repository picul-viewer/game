#ifndef __core_memory_h_included_
#define __core_memory_h_included_

#include <core/types.h>
#include <core/macros.h>

#include <nmmintrin.h>

namespace memory
{

inline void set( pvoid ptr, uptr size, u8 value )
{
	if ( size < 16 )
	{
		for ( pbyte l = (pbyte)ptr, r = l + size; l != r; ++l )
			*l = value;
		return;
	}

	__m128i val128 = _mm_set1_epi8( value );

	uptr curr_ptr = (uptr)ptr;
	uptr max_ptr = curr_ptr + size;

	if ( !aligned( curr_ptr, 16 ) )
	{
		_mm_storeu_si128( (__m128i*)curr_ptr, val128 );
		curr_ptr = ( curr_ptr + 16 ) & 0xFFFFFFF0;
	}

	for ( ; curr_ptr + 16 < max_ptr; curr_ptr += 16 )
		_mm_store_si128( (__m128i*)curr_ptr, val128 );

	if ( !aligned( max_ptr, 16 ) )
		_mm_storeu_si128( (__m128i*)( max_ptr - 16 ), val128 );
}

inline void zero( pvoid ptr, uptr size )
{
	if ( size < 16 )
	{
		if ( size >= 8 )
			*(u64*)ptr = 0;
		if ( size >= 12 )
			*( (u32*)ptr + 2 ) = 0;
		if ( size >= 14 )
			*( (u16*)ptr + 6 ) = 0;
		if ( size >= 15 )
			*( (u8*)ptr + 14 ) = 0;
		return;
	}

	__m128i val128 = _mm_setzero_si128( );

	uptr curr_ptr = (uptr)ptr;
	uptr max_ptr = curr_ptr + size;

	if ( !aligned( curr_ptr, 16 ) )
	{
		_mm_storeu_si128( (__m128i*)curr_ptr, val128 );
		curr_ptr = ( curr_ptr + 16 ) & 0xFFFFFFF0;
	}

	for ( ; curr_ptr + 16 < max_ptr; curr_ptr += 16 )
		_mm_store_si128( (__m128i*)curr_ptr, val128 );

	if ( !aligned( max_ptr, 16 ) )
		_mm_storeu_si128( (__m128i*)( max_ptr - 16 ), val128 );
}

// 'dest' and 'src' must be aligned properly, and 'size' % 16 should equal to 0
inline void copy_atoa( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( aligned( dest, 16 ) );
	ASSERT( size != 0 );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_store_ps( (float*)dest + i / 4, _mm_load_ps( (float*)src + i / 4 ) );
}

// 'dest' must be aligned properly, and 'size' % 16 should equal to 0
inline void copy_utoa( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( aligned( dest, 16 ) );
	ASSERT( size != 0 );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_store_ps( (float*)dest + i / 4, _mm_loadu_ps( (float*)src + i / 4 ) );
}

// 'src' must be aligned properly, and 'size' % 16 should equal to 0
inline void copy_atou( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( aligned( dest, 16 ) );
	ASSERT( size != 0 );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_storeu_ps( (float*)dest + i / 4, _mm_load_ps( (float*)src + i / 4 ) );
}

// 'size' % 16 should equal to 0
inline void copy_utou( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( aligned( dest, 16 ) );
	ASSERT( size != 0 );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_storeu_ps( (float*)dest + i / 4, _mm_loadu_ps( (float*)src + i / 4 ) );
}

}

#endif // #ifndef __core_memory_h_included_