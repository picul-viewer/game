#include <Windows.h>

#include "interlocked.h"

s32 interlocked_exchange( mt_s32& left, s32 right )
{
	return InterlockedExchange( (volatile LONG*)&left, (LONG)right );
}

s64 interlocked_exchange( mt_s64& left, s64 right )
{
	return InterlockedExchange64( &left, right );
}

u32 interlocked_exchange( mt_u32& left, u32 right )
{
	return InterlockedExchange( (volatile LONG*)&left, right );
}

u64 interlocked_exchange( mt_u64& left, u64 right )
{
	return InterlockedExchange64( (mt_s64*)&left, right );
}

pvoid interlocked_exchange_pointer( mt_pvoid& left, pvoid right )
{
	return InterlockedExchangePointer( &left, right );
}

s32 interlocked_inc( mt_s32& left )
{
	return InterlockedIncrement( (volatile LONG*)&left );
}

s64 interlocked_inc( mt_s64& left )
{
	return InterlockedIncrement64( &left );
}

u32 interlocked_inc( mt_u32& left )
{
	return InterlockedIncrement( (volatile LONG*)&left );
}

u64 interlocked_inc( mt_u64& left )
{
	return InterlockedIncrement64( (mt_s64*)&left );
}

s32 interlocked_dec( mt_s32& left )
{
	return InterlockedDecrement( (volatile LONG*)&left );
}

s64 interlocked_dec( mt_s64& left )
{
	return InterlockedDecrement64( &left );
}

u32 interlocked_dec( mt_u32& left )
{
	return InterlockedDecrement( (volatile LONG*)&left );
}

u64 interlocked_dec( mt_u64& left )
{
	return InterlockedDecrement64( (mt_s64*)&left );
}

s32 interlocked_add( mt_s32& left, s32 right )
{
	return InterlockedAdd( (volatile LONG*)&left, right );
}

s64 interlocked_add( mt_s64& left, s64 right )
{
	return InterlockedAdd64( &left, right );
}

u32 interlocked_add( mt_u32& left, u32 right )
{
	return InterlockedAdd( (volatile LONG*)&left, right );
}

u64 interlocked_add( mt_u64& left, u64 right )
{
	return InterlockedAdd64( (mt_s64*)&left, right );
}

s32 interlocked_or( mt_s32& left, s32 right )
{
	return InterlockedOr( (volatile LONG*)&left, right );
}

s64 interlocked_or( mt_s64& left, s64 right )
{
	return InterlockedOr64( &left, right );
}

u32 interlocked_or( mt_u32& left, u32 right )
{
	return InterlockedOr( (volatile LONG*)&left, right );
}

u64 interlocked_or( mt_u64& left, u64 right )
{
	return InterlockedOr64( (mt_s64*)&left, right );
}

s32 interlocked_and( mt_s32& left, s32 right )
{
	return InterlockedAnd( (volatile LONG*)&left, right );
}

s64 interlocked_and( mt_s64& left, s64 right )
{
	return InterlockedAnd64( &left, right );
}

u32 interlocked_and( mt_u32& left, u32 right )
{
	return InterlockedAnd( (volatile LONG*)&left, right );
}

u64 interlocked_and( mt_u64& left, u64 right )
{
	return InterlockedAnd64( (mt_s64*)&left, right );
}

s32 interlocked_xor( mt_s32& left, s32 right )
{
	return InterlockedXor( (volatile LONG*)&left, right );
}

s64 interlocked_xor( mt_s64& left, s64 right )
{
	return InterlockedXor64( &left, right );
}

u32 interlocked_xor( mt_u32& left, u32 right )
{
	return InterlockedXor( (volatile LONG*)&left, right );
}

u64 interlocked_xor( mt_u64& left, u64 right )
{
	return InterlockedXor64( (mt_s64*)&left, right );
}