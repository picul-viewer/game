#ifndef __core_memory_h_included_
#define __core_memory_h_included_

#include <types.h>
#include <macros.h>

#include <nmmintrin.h>

namespace memory
{

template<u32 Index>
inline void small_set( pbyte dest, u64 value )
{
	static_assert( Index < 16, "Index should be less than 16" );

	if ( Index & 8 )
	{
		*(u64*)dest = value;

		dest += 8;
	}
	
	if ( Index & 4 )
	{
		*(u32*)dest = (u32)value;

		dest += 4;
	}
	
	if ( Index & 2 )
	{
		*(u16*)dest = (u16)value;

		dest += 2;
	}
	
	if ( Index & 1 )
	{
		*(u8*)dest = (u8)value;
	}
}

inline void set( pvoid ptr, uptr size, u8 value )
{
	ASSERT( ptr );

	if ( size < 16 )
	{
		typedef void(*set_functor_type)(pbyte, u64);

		static const set_functor_type set_functors[] = {
			small_set<0>, small_set<1>, small_set<2>, small_set<3>,
			small_set<4>, small_set<5>, small_set<6>, small_set<7>,
			small_set<8>, small_set<9>, small_set<10>, small_set<11>,
			small_set<12>, small_set<13>, small_set<14>, small_set<15>
		};

		set_functors[size % 16]( (pbyte)ptr, 0x0101010101010101 * value );

		return;
	}

	__m128i val128 = _mm_set1_epi8( value );

	if ( !aligned( (uptr)ptr, 16 ) )
	{
		_mm_storeu_si128( (__m128i*)ptr, val128 );

		uptr first_line = 16 - ( (uptr)ptr & 0xF );

		ptr = ( (pbyte)ptr + first_line );
		size -= first_line;
	}

	__m128i* data = (__m128i*)ptr;

	for ( uptr i = size / 16; i; --i )
		_mm_store_si128( data++, val128 );

	if ( size % 16 )
		_mm_storeu_si128( (__m128i*)( (pbyte)ptr + size - 16 ), val128 );
}


template<u32 Index>
inline void small_zero( pbyte dest )
{
	static_assert( Index < 16, "Index should be less than 16" );

	if ( Index & 8 )
	{
		*(u64*)dest = 0;

		dest += 8;
	}
	
	if ( Index & 4 )
	{
		*(u32*)dest = 0;

		dest += 4;
	}
	
	if ( Index & 2 )
	{
		*(u16*)dest = 0;

		dest += 2;
	}
	
	if ( Index & 1 )
	{
		*(u8*)dest = 0;
	}
}

inline void zero( pvoid ptr, uptr size )
{
	ASSERT( ptr );

	if ( size < 16 )
	{
		typedef void(*zero_functor_type)(pbyte);

		static const zero_functor_type zero_functors[] = {
			small_zero<0>, small_zero<1>, small_zero<2>, small_zero<3>,
			small_zero<4>, small_zero<5>, small_zero<6>, small_zero<7>,
			small_zero<8>, small_zero<9>, small_zero<10>, small_zero<11>,
			small_zero<12>, small_zero<13>, small_zero<14>, small_zero<15>
		};

		zero_functors[size % 16]( (pbyte)ptr );

		return;
	}

	__m128i val128 = _mm_setzero_si128( );

	if ( !aligned( (uptr)ptr, 16 ) )
	{
		_mm_storeu_si128( (__m128i*)ptr, val128 );

		uptr first_line = 16 - ( (uptr)ptr & 0xF );

		ptr = ( (pbyte)ptr + first_line );
		size -= first_line;
	}

	__m128i* data = (__m128i*)ptr;

	for ( uptr i = size / 16; i; --i )
		_mm_store_si128( data++, val128 );

	if ( size % 16 )
		_mm_storeu_si128( (__m128i*)( (pbyte)ptr + size - 16 ), val128 );
}


template<u32 Index>
inline void small_copy( pbyte dest, pcbyte src )
{
	static_assert( Index < 16, "Index should be less than 16" );

	if ( Index & 8 )
	{
		*(u64*)dest = *(u64*)src;

		dest += 8;
		src += 8;
	}
	
	if ( Index & 4 )
	{
		*(u32*)dest = *(u32*)src;

		dest += 4;
		src += 4;
	}
	
	if ( Index & 2 )
	{
		*(u16*)dest = *(u16*)src;

		dest += 2;
		src += 2;
	}
	
	if ( Index & 1 )
	{
		*(u8*)dest = *(u8*)src;
	}
}

inline void copy( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( dest );
	ASSERT( src );

	if ( size < 16 )
	{
		typedef void(*copy_functor_type)(pbyte, pcbyte);

		static const copy_functor_type copy_functors[] = {
			small_copy<0>, small_copy<1>, small_copy<2>, small_copy<3>,
			small_copy<4>, small_copy<5>, small_copy<6>, small_copy<7>,
			small_copy<8>, small_copy<9>, small_copy<10>, small_copy<11>,
			small_copy<12>, small_copy<13>, small_copy<14>, small_copy<15>
		};

		copy_functors[size % 16]( (pbyte)dest, (pcbyte)src );

		return;
	}

	if ( !aligned( dest, 16 ) )
	{
		_mm_storeu_si128( (__m128i*)dest, _mm_loadu_si128( (__m128i const*)src ) );

		uptr first_line = 16 - ( (uptr)dest & 0xF );

		dest = ( (pbyte)dest + first_line );
		src = ( (pbyte)src + first_line );
		size -= first_line;
	}
	
	__m128i*		d = (__m128i*)dest;
	__m128i const*	s = (__m128i const*)src;

	for ( uptr i = size / 16; i; --i )
		_mm_storeu_si128( d++, _mm_loadu_si128( s++ ) );

	if ( size % 16 )
		_mm_storeu_si128( (__m128i*)( (pbyte)dest + size - 16 ), _mm_loadu_si128( (__m128i const*)( (pbyte)src + size - 16 ) ) );
}


// 'dest' and 'src' must be aligned properly, and 'size' % 16 should equal to 0
inline void copy_aligned_to_aligned_mul16( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( dest );
	ASSERT( aligned( dest, 16 ) );
	ASSERT( src );
	ASSERT( aligned( src, 16 ) );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_store_ps( (float*)dest + i / 4, _mm_load_ps( (float*)src + i / 4 ) );
}

// 'dest' must be aligned properly, and 'size' % 16 should equal to 0
inline void copy_to_aligned_mul16( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( dest );
	ASSERT( aligned( dest, 16 ) );
	ASSERT( src );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_store_ps( (float*)dest + i / 4, _mm_loadu_ps( (float*)src + i / 4 ) );
}

// 'src' must be aligned properly, and 'size' % 16 should equal to 0
inline void copy_aligned_mul16( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( dest );
	ASSERT( src );
	ASSERT( aligned( src, 16 ) );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_storeu_ps( (float*)dest + i / 4, _mm_load_ps( (float*)src + i / 4 ) );
}

// 'size' % 16 should equal to 0
inline void copy_mul16( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( dest );
	ASSERT( src );
	ASSERT( aligned( dest, 16 ) );
	ASSERT( size % 16 == 0 );

	for ( u32 i = 0; i < size; i += 16 )
		_mm_storeu_ps( (float*)dest + i / 4, _mm_loadu_ps( (float*)src + i / 4 ) );
}


template<u32 Index>
inline bool small_equal( pcbyte left, pcbyte right )
{
	static_assert( Index < 16, "Index should be less than 16" );

	if ( Index & 8 )
	{
		if ( *(u64*)left != *(u64*)right )
			return false;

		left += 8;
		right += 8;
	}
	
	if ( Index & 4 )
	{
		if ( *(u32*)left != *(u32*)right )
			return false;

		left += 4;
		right += 4;
	}
	
	if ( Index & 2 )
	{
		if ( *(u16*)left != *(u16*)right )
			return false;

		left += 2;
		right += 2;
	}
	
	if ( Index & 1 )
	{
		if ( *(u8*)left != *(u8*)right )
			return false;
	}

	return true;
}

inline bool equal( pcvoid left, pcvoid right, uptr size )
{
	ASSERT( left );
	ASSERT( right );

	if ( size < 16 )
	{
		typedef bool(*equal_functor_type)(pcbyte, pcbyte);

		static const equal_functor_type equal_functors[] = {
			small_equal<0>, small_equal<1>, small_equal<2>, small_equal<3>,
			small_equal<4>, small_equal<5>, small_equal<6>, small_equal<7>,
			small_equal<8>, small_equal<9>, small_equal<10>, small_equal<11>,
			small_equal<12>, small_equal<13>, small_equal<14>, small_equal<15>
		};

		return equal_functors[size % 16]( (pcbyte)left, (pcbyte)right );
	}

	__m128i* l = (__m128i*)left;
	__m128i* r = (__m128i*)right;

	for ( uptr i = size / 16; i; --i )
	{
		__m128i ldata = _mm_loadu_si128( l );
		__m128i rdata = _mm_loadu_si128( r );

		if ( !_mm_testc_si128( ldata, rdata ) )
			return false;

		++l;
		++r;
	}

	if ( size % 16 )
	{
		__m128i ldata = _mm_loadu_si128( (__m128i const*)( (pcbyte)left + size - 16 ) );
		__m128i rdata = _mm_loadu_si128( (__m128i const*)( (pcbyte)right + size - 16 ) );

		if ( !_mm_testc_si128( ldata, rdata ) )
			return false;
	}

	return true;
}

template<u32 Index>
inline bool small_less( pcbyte left, pcbyte right )
{
	static_assert( Index < 16, "Index should be less than 16" );

	if ( Index & 8 )
	{
		if ( *(u64*)left != *(u64*)right )
			return *(u64*)left < *(u64*)right;

		left += 8;
		right += 8;
	}
	
	if ( Index & 4 )
	{
		if ( *(u32*)left != *(u32*)right )
			return *(u32*)left < *(u32*)right;

		left += 4;
		right += 4;
	}
	
	if ( Index & 2 )
	{
		if ( *(u16*)left != *(u16*)right )
			return *(u16*)left < *(u16*)right;

		left += 2;
		right += 2;
	}
	
	if ( Index & 1 )
	{
		if ( *(u8*)left != *(u8*)right )
			return *(u8*)left < *(u8*)right;
	}

	return false;
}

inline bool less( pcvoid left, pcvoid right, uptr size )
{
	ASSERT( left );
	ASSERT( right );

	if ( size < 16 )
	{
		typedef bool(*less_functor_type)(pcbyte, pcbyte);

		static const less_functor_type less_functors[] = {
			small_less<0>, small_less<1>, small_less<2>, small_less<3>,
			small_less<4>, small_less<5>, small_less<6>, small_less<7>,
			small_less<8>, small_less<9>, small_less<10>, small_less<11>,
			small_less<12>, small_less<13>, small_less<14>, small_less<15>
		};

		return less_functors[size % 16]( (pcbyte)left, (pcbyte)right );
	}

	__m128i* l = (__m128i*)left;
	__m128i* r = (__m128i*)right;

	for ( uptr i = size / 16; i; --i )
	{
		__m128i ldata = _mm_loadu_si128( l );
		__m128i rdata = _mm_loadu_si128( r );

		if ( !_mm_testc_si128( ldata, rdata ) )
		{
			// TODO: search for better way to handle this.
			if ( *(u64*)l != *(u64*)r )
				return *(u64*)l < *(u64*)r;

			return *( (u64*)l + 1 ) < *( (u64*)r + 1 );
		}

		++l;
		++r;
	}
	
	if ( size % 16 )
	{
		__m128i ldata = _mm_loadu_si128( (__m128i const*)( (pcbyte)left + size - 16 ) );
		__m128i rdata = _mm_loadu_si128( (__m128i const*)( (pcbyte)right + size - 16 ) );

		
		if ( !_mm_testc_si128( ldata, rdata ) )
		{
			if ( *(u64*)l != *(u64*)r )
				return *(u64*)l < *(u64*)r;

			return *( (u64*)l + 1 ) < *( (u64*)r + 1 );
		}
	}

	return true;
}

template<u32 Index>
inline bool small_greater( pcbyte left, pcbyte right )
{
	static_assert( Index < 16, "Index should be less than 16" );

	if ( Index & 8 )
	{
		if ( *(u64*)left != *(u64*)right )
			return *(u64*)left > *(u64*)right;

		left += 8;
		right += 8;
	}
	
	if ( Index & 4 )
	{
		if ( *(u32*)left != *(u32*)right )
			return *(u32*)left > *(u32*)right;

		left += 4;
		right += 4;
	}
	
	if ( Index & 2 )
	{
		if ( *(u16*)left != *(u16*)right )
			return *(u16*)left > *(u16*)right;

		left += 2;
		right += 2;
	}
	
	if ( Index & 1 )
	{
		if ( *(u8*)left != *(u8*)right )
			return *(u8*)left > *(u8*)right;
	}

	return false;
}

inline bool greater( pcvoid left, pcvoid right, uptr size )
{
	ASSERT( left );
	ASSERT( right );

	if ( size < 16 )
	{
		typedef bool(*greater_functor_type)(pcbyte, pcbyte);

		static const greater_functor_type greater_functors[] = {
			small_greater<0>, small_greater<1>, small_greater<2>, small_greater<3>,
			small_greater<4>, small_greater<5>, small_greater<6>, small_greater<7>,
			small_greater<8>, small_greater<9>, small_greater<10>, small_greater<11>,
			small_greater<12>, small_greater<13>, small_greater<14>, small_greater<15>
		};

		return greater_functors[size % 16]( (pcbyte)left, (pcbyte)right );
	}

	__m128i* l = (__m128i*)left;
	__m128i* r = (__m128i*)right;

	for ( uptr i = size / 16; i; --i )
	{
		__m128i ldata = _mm_loadu_si128( l );
		__m128i rdata = _mm_loadu_si128( r );

		if ( !_mm_testc_si128( ldata, rdata ) )
		{
			// TODO: search for better way to handle this.
			if ( *(u64*)l != *(u64*)r )
				return *(u64*)l > *(u64*)r;

			return *( (u64*)l + 1 ) > *( (u64*)r + 1 );
		}

		++l;
		++r;
	}
	
	if ( size % 16 )
	{
		__m128i ldata = _mm_loadu_si128( (__m128i const*)( (pcbyte)left + size - 16 ) );
		__m128i rdata = _mm_loadu_si128( (__m128i const*)( (pcbyte)right + size - 16 ) );

		
		if ( !_mm_testc_si128( ldata, rdata ) )
		{
			if ( *(u64*)l != *(u64*)r )
				return *(u64*)l > *(u64*)r;

			return *( (u64*)l + 1 ) > *( (u64*)r + 1 );
		}
	}

	return true;
}

} // namespace memory

#endif // #ifndef __core_memory_h_included_