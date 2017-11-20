#ifndef __core_interlocked_h_included_
#define __core_interlocked_h_included_

#include <core/types.h>
#include <Windows.h>

typedef LONG volatile mt_s32;
typedef LONG64 volatile mt_s64;
typedef unsigned int volatile mt_u32;
typedef unsigned long long volatile mt_u64;
typedef pvoid volatile mt_pvoid;

inline s32 interlocked_exchange( mt_s32& left, s32 right )
{
	return InterlockedExchange( &left, right );
}

inline s64 interlocked_exchange( mt_s64& left, s64 right )
{
	return InterlockedExchange64( &left, right );
}

inline u32 interlocked_exchange( mt_u32& left, u32 right )
{
	return InterlockedExchange( (mt_s32*)&left, right );
}

inline u64 interlocked_exchange( mt_u64& left, u64 right )
{
	return InterlockedExchange64( (mt_s64*)&left, right );
}

inline pvoid interlocked_exchange_pointer( mt_pvoid& left, pvoid right )
{
	return InterlockedExchangePointer( &left, right );
}

inline s32 interlocked_inc( mt_s32& left )
{
	return InterlockedIncrement( &left );
}

inline s64 interlocked_inc( mt_s64& left )
{
	return InterlockedIncrement64( &left );
}

inline u32 interlocked_inc( mt_u32& left )
{
	return InterlockedIncrement( (mt_s32*)&left );
}

inline u64 interlocked_inc( mt_u64& left )
{
	return InterlockedIncrement64( (mt_s64*)&left );
}

inline s32 interlocked_dec( mt_s32& left )
{
	return InterlockedDecrement( &left );
}

inline s64 interlocked_dec( mt_s64& left )
{
	return InterlockedDecrement64( &left );
}

inline u32 interlocked_dec( mt_u32& left )
{
	return InterlockedDecrement( (mt_s32*)&left );
}

inline u64 interlocked_dec( mt_u64& left )
{
	return InterlockedDecrement64( (mt_s64*)&left );
}

inline s32 interlocked_add( mt_s32& left, s32 right )
{
	return InterlockedAdd( &left, right );
}

inline s64 interlocked_add( mt_s64& left, s64 right )
{
	return InterlockedAdd64( &left, right );
}

inline u32 interlocked_add( mt_u32& left, u32 right )
{
	return InterlockedAdd( (mt_s32*)&left, right );
}

inline u64 interlocked_add( mt_u64& left, u64 right )
{
	return InterlockedAdd64( (mt_s64*)&left, right );
}

#endif