#include "interlocked.h"
#include <Windows.h>

s16 interlocked_exchange( mt_s16& dest, s16 const value )
{
	return _InterlockedExchange16( (volatile SHORT*)&dest, value );
}

u16 interlocked_exchange( mt_u16& dest, u16 const value )
{
	return _InterlockedExchange16( (volatile SHORT*)&dest, value );
}

s32 interlocked_exchange( mt_s32& dest, s32 const value )
{
	return _InterlockedExchange( (volatile LONG*)&dest, value );
}

u32 interlocked_exchange( mt_u32& dest, u32 const value )
{
	return _InterlockedExchange( (volatile LONG*)&dest, value );
}

s64 interlocked_exchange( mt_s64& dest, s64 const value )
{
	return _InterlockedExchange64( (volatile LONG64*)&dest, value );
}

u64 interlocked_exchange( mt_u64& dest, u64 const value )
{
	return _InterlockedExchange64( (volatile LONG64*)&dest, value );
}

s16 interlocked_compare_exchange( mt_s16& dest, s16 const value, s16 const comparand )
{
	return _InterlockedCompareExchange16( (volatile SHORT*)&dest, value, comparand );
}

u16 interlocked_compare_exchange( mt_u16& dest, u16 const value, u16 const comparand )
{
	return _InterlockedCompareExchange16( (volatile SHORT*)&dest, value, comparand );
}

s32 interlocked_compare_exchange( mt_s32& dest, s32 const value, s32 const comparand )
{
	return _InterlockedCompareExchange( (volatile LONG*)&dest, value, comparand );
}

u32 interlocked_compare_exchange( mt_u32& dest, u32 const value, u32 const comparand )
{
	return _InterlockedCompareExchange( (volatile LONG*)&dest, value, comparand );
}

s64 interlocked_compare_exchange( mt_s64& dest, s64 const value, s64 const comparand )
{
	return _InterlockedCompareExchange64( (volatile LONG64*)&dest, value, comparand );
}

u64 interlocked_compare_exchange( mt_u64& dest, u64 const value, u64 const comparand )
{
	return _InterlockedCompareExchange64( (volatile LONG64*)&dest, value, comparand );
}

s16 interlocked_inc( mt_s16& left )
{
	return _InterlockedIncrement16( (volatile SHORT*)&left );
}

u16 interlocked_inc( mt_u16& left )
{
	return _InterlockedIncrement16( (volatile SHORT*)&left );
}

s32 interlocked_inc( mt_s32& left )
{
	return _InterlockedIncrement( (volatile LONG*)&left );
}

u32 interlocked_inc( mt_u32& left )
{
	return _InterlockedIncrement( (volatile LONG*)&left );
}

s64 interlocked_inc( mt_s64& left )
{
	return _InterlockedIncrement64( (volatile LONG64*)&left );
}

u64 interlocked_inc( mt_u64& left )
{
	return _InterlockedIncrement64( (volatile LONG64*)&left );
}

s16 interlocked_dec( mt_s16& left )
{
	return _InterlockedDecrement16( (volatile SHORT*)&left );
}

u16 interlocked_dec( mt_u16& left )
{
	return _InterlockedDecrement16( (volatile SHORT*)&left );
}

s32 interlocked_dec( mt_s32& left )
{
	return _InterlockedDecrement( (volatile LONG*)&left );
}

u32 interlocked_dec( mt_u32& left )
{
	return _InterlockedDecrement( (volatile LONG*)&left );
}

s64 interlocked_dec( mt_s64& left )
{
	return _InterlockedDecrement64( (volatile LONG64*)&left );
}

u64 interlocked_dec( mt_u64& left )
{
	return _InterlockedDecrement64( (volatile LONG64*)&left );
}

s32 interlocked_add( mt_s32& left, s32 const right )
{
	return _InlineInterlockedAdd( (volatile LONG*)&left, right );
}

u32 interlocked_add( mt_u32& left, u32 const right )
{
	return _InlineInterlockedAdd( (volatile LONG*)&left, right );
}

s64 interlocked_add( mt_s64& left, s64 const right )
{
	return _InlineInterlockedAdd64( (volatile LONG64*)&left, right );
}

u64 interlocked_add( mt_u64& left, u64 const right )
{
	return _InlineInterlockedAdd64( (volatile LONG64*)&left, right );
}

s16 interlocked_or( mt_s16& left, s16 const right )
{
	return _InterlockedOr16( (volatile SHORT*)&left, right );
}

u16 interlocked_or( mt_u16& left, u16 const right )
{
	return _InterlockedOr16( (volatile SHORT*)&left, right );
}

s32 interlocked_or( mt_s32& left, s32 const right )
{
	return _InterlockedOr( (volatile LONG*)&left, right );
}

u32 interlocked_or( mt_u32& left, u32 const right )
{
	return _InterlockedOr( (volatile LONG*)&left, right );
}

s64 interlocked_or( mt_s64& left, s64 const right )
{
	return _InterlockedOr64( (volatile LONG64*)&left, right );
}

u64 interlocked_or( mt_u64& left, u64 const right )
{
	return _InterlockedOr64( (volatile LONG64*)&left, right );
}

s16 interlocked_and( mt_s16& left, s16 const right )
{
	return _InterlockedAnd16( (volatile SHORT*)&left, right );
}

u16 interlocked_and( mt_u16& left, u16 const right )
{
	return _InterlockedAnd16( (volatile SHORT*)&left, right );
}

s32 interlocked_and( mt_s32& left, s32 const right )
{
	return _InterlockedAnd( (volatile LONG*)&left, right );
}

u32 interlocked_and( mt_u32& left, u32 const right )
{
	return _InterlockedAnd( (volatile LONG*)&left, right );
}

s64 interlocked_and( mt_s64& left, s64 const right )
{
	return _InterlockedAnd64( (volatile LONG64*)&left, right );
}

u64 interlocked_and( mt_u64& left, u64 const right )
{
	return _InterlockedAnd64( (volatile LONG64*)&left, right );
}

s16 interlocked_xor( mt_s16& left, s16 const right )
{
	return _InterlockedXor16( (volatile SHORT*)&left, right );
}

u16 interlocked_xor( mt_u16& left, u16 const right )
{
	return _InterlockedXor16( (volatile SHORT*)&left, right );
}

s32 interlocked_xor( mt_s32& left, s32 const right )
{
	return _InterlockedXor( (volatile LONG*)&left, right );
}

u32 interlocked_xor( mt_u32& left, u32 const right )
{
	return _InterlockedXor( (volatile LONG*)&left, right );
}

s64 interlocked_xor( mt_s64& left, s64 const right )
{
	return _InterlockedXor64( (volatile LONG64*)&left, right );
}

u64 interlocked_xor( mt_u64& left, u64 const right )
{
	return _InterlockedXor64( (volatile LONG64*)&left, right );
}

u32 interlocked_bit_test_and_set( mt_u32& value, u32 const bit_index )
{
	return _interlockedbittestandset( (volatile LONG*)&value, bit_index );
}

u32 interlocked_bit_test_and_reset( mt_u32& value, u32 const bit_index )
{
	return _interlockedbittestandreset( (volatile LONG*)&value, bit_index );
}

u64 interlocked_bit_test_and_set( mt_u64& value, u64 const bit_index )
{
	return _interlockedbittestandset64( (volatile LONG64*)&value, bit_index );
}

u64 interlocked_bit_test_and_reset( mt_u64& value, u64 const bit_index )
{
	return _interlockedbittestandreset64( (volatile LONG64*)&value, bit_index );
}

void load_fence( )
{
	_mm_lfence( );
}

void store_fence( )
{
	_mm_sfence( );
}

void full_fence( )
{
	_mm_mfence( );
}