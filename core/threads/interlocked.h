#ifndef __core_interlocked_h_included_
#define __core_interlocked_h_included_

#include <types.h>

typedef s32 volatile mt_s32;
typedef s64 volatile mt_s64;
typedef u32 volatile mt_u32;
typedef u64 volatile mt_u64;
typedef pvoid volatile mt_pvoid;

s32 interlocked_exchange( mt_s32& left, s32 right );
s64 interlocked_exchange( mt_s64& left, s64 right );
u32 interlocked_exchange( mt_u32& left, u32 right );
u64 interlocked_exchange( mt_u64& left, u64 right );
pvoid interlocked_exchange_pointer( mt_pvoid& left, pvoid right );
s32 interlocked_inc( mt_s32& left );
s64 interlocked_inc( mt_s64& left );
u32 interlocked_inc( mt_u32& left );
u64 interlocked_inc( mt_u64& left );
s32 interlocked_dec( mt_s32& left );
s64 interlocked_dec( mt_s64& left );
u32 interlocked_dec( mt_u32& left );
u64 interlocked_dec( mt_u64& left );
s32 interlocked_add( mt_s32& left, s32 right );
s64 interlocked_add( mt_s64& left, s64 right );
u32 interlocked_add( mt_u32& left, u32 right );
u64 interlocked_add( mt_u64& left, u64 right );
s32 interlocked_or( mt_s32& left, s32 right );
s64 interlocked_or( mt_s64& left, s64 right );
u32 interlocked_or( mt_u32& left, u32 right );
u64 interlocked_or( mt_u64& left, u64 right );
s32 interlocked_and( mt_s32& left, s32 right );
s64 interlocked_and( mt_s64& left, s64 right );
u32 interlocked_and( mt_u32& left, u32 right );
u64 interlocked_and( mt_u64& left, u64 right );
s32 interlocked_xor( mt_s32& left, s32 right );
s64 interlocked_xor( mt_s64& left, s64 right );
u32 interlocked_xor( mt_u32& left, u32 right );
u64 interlocked_xor( mt_u64& left, u64 right );

#endif