#ifndef __core_interlocked_h_included_
#define __core_interlocked_h_included_

#include <types.h>

typedef s32 volatile mt_s32;
typedef u32 volatile mt_u32;
typedef pvoid volatile mt_pvoid;

s32 interlocked_exchange( mt_s32& left, s32 right );
u32 interlocked_exchange( mt_u32& left, u32 right );
s32 interlocked_compare_exchange( mt_s32& dest, s32 value, s32 comparand );
u32 interlocked_compare_exchange( mt_u32& dest, u32 value, u32 comparand );
s32 interlocked_inc( mt_s32& left );
u32 interlocked_inc( mt_u32& left );
s32 interlocked_dec( mt_s32& left );
u32 interlocked_dec( mt_u32& left );
s32 interlocked_add( mt_s32& left, s32 right );
u32 interlocked_add( mt_u32& left, u32 right );
s32 interlocked_or( mt_s32& left, s32 right );
u32 interlocked_or( mt_u32& left, u32 right );
s32 interlocked_and( mt_s32& left, s32 right );
u32 interlocked_and( mt_u32& left, u32 right );
s32 interlocked_xor( mt_s32& left, s32 right );
u32 interlocked_xor( mt_u32& left, u32 right );

#endif