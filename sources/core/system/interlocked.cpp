#include <Windows.h>

#include "interlocked.h"

s32 interlocked_exchange( mt_s32& left, s32 right )
{
	return InterlockedExchange( (volatile LONG*)&left, (LONG)right );
}

u32 interlocked_exchange( mt_u32& left, u32 right )
{
	return InterlockedExchange( (volatile LONG*)&left, right );
}

s32 interlocked_compare_exchange( mt_s32& dest, s32 value, s32 comparand )
{
	return InterlockedCompareExchange( (volatile LONG*)&dest, (LONG)value, (LONG)comparand );
}

u32 interlocked_compare_exchange( mt_u32& dest, u32 value, u32 comparand )
{
	return InterlockedCompareExchange( (volatile LONG*)&dest, (LONG)value, (LONG)comparand );
}

s32 interlocked_inc( mt_s32& left )
{
	return InterlockedIncrement( (volatile LONG*)&left );
}

u32 interlocked_inc( mt_u32& left )
{
	return InterlockedIncrement( (volatile LONG*)&left );
}

s32 interlocked_dec( mt_s32& left )
{
	return InterlockedDecrement( (volatile LONG*)&left );
}

u32 interlocked_dec( mt_u32& left )
{
	return InterlockedDecrement( (volatile LONG*)&left );
}

s32 interlocked_add( mt_s32& left, s32 right )
{
	return InterlockedAdd( (volatile LONG*)&left, right );
}

u32 interlocked_add( mt_u32& left, u32 right )
{
	return InterlockedAdd( (volatile LONG*)&left, right );
}

s32 interlocked_or( mt_s32& left, s32 right )
{
	return InterlockedOr( (volatile LONG*)&left, right );
}

u32 interlocked_or( mt_u32& left, u32 right )
{
	return InterlockedOr( (volatile LONG*)&left, right );
}

s32 interlocked_and( mt_s32& left, s32 right )
{
	return InterlockedAnd( (volatile LONG*)&left, right );
}

u32 interlocked_and( mt_u32& left, u32 right )
{
	return InterlockedAnd( (volatile LONG*)&left, right );
}

s32 interlocked_xor( mt_s32& left, s32 right )
{
	return InterlockedXor( (volatile LONG*)&left, right );
}

u32 interlocked_xor( mt_u32& left, u32 right )
{
	return InterlockedXor( (volatile LONG*)&left, right );
}