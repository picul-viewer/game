#ifndef GUARD_CORE_INTERLOCKED_H_INCLUDED
#define GUARD_CORE_INTERLOCKED_H_INCLUDED

#include <types.h>

typedef s16 volatile mt_s16;
typedef u16 volatile mt_u16;
typedef s32 volatile mt_s32;
typedef u32 volatile mt_u32;
typedef s64 volatile mt_s64;
typedef u64 volatile mt_u64;

s16 interlocked_exchange( mt_s16& dest, s16 const value );
u16 interlocked_exchange( mt_u16& dest, u16 const value );
s32 interlocked_exchange( mt_s32& dest, s32 const value );
u32 interlocked_exchange( mt_u32& dest, u32 const value );
s64 interlocked_exchange( mt_s64& dest, s64 const value );
u64 interlocked_exchange( mt_u64& dest, u64 const value );
s16 interlocked_compare_exchange( mt_s16& dest, s16 const value, s16 const comparand );
u16 interlocked_compare_exchange( mt_u16& dest, u16 const value, u16 const comparand );
s32 interlocked_compare_exchange( mt_s32& dest, s32 const value, s32 const comparand );
u32 interlocked_compare_exchange( mt_u32& dest, u32 const value, u32 const comparand );
s64 interlocked_compare_exchange( mt_s64& dest, s64 const value, s64 const comparand );
u64 interlocked_compare_exchange( mt_u64& dest, u64 const value, u64 const comparand );
s16 interlocked_inc( mt_s16& left );
u16 interlocked_inc( mt_u16& left );
s32 interlocked_inc( mt_s32& left );
u32 interlocked_inc( mt_u32& left );
s64 interlocked_inc( mt_s64& left );
u64 interlocked_inc( mt_u64& left );
s16 interlocked_dec( mt_s16& left );
u16 interlocked_dec( mt_u16& left );
s32 interlocked_dec( mt_s32& left );
u32 interlocked_dec( mt_u32& left );
s64 interlocked_dec( mt_s64& left );
u64 interlocked_dec( mt_u64& left );
s32 interlocked_exchange_add( mt_s32& left, s32 const right );
u32 interlocked_exchange_add( mt_u32& left, u32 const right );
s64 interlocked_exchange_add( mt_s64& left, s64 const right );
u64 interlocked_exchange_add( mt_u64& left, u64 const right );
s32 interlocked_add( mt_s32& left, s32 const right );
u32 interlocked_add( mt_u32& left, u32 const right );
s64 interlocked_add( mt_s64& left, s64 const right );
u64 interlocked_add( mt_u64& left, u64 const right );
s16 interlocked_or( mt_s16& left, s16 const right );
u16 interlocked_or( mt_u16& left, u16 const right );
s32 interlocked_or( mt_s32& left, s32 const right );
u32 interlocked_or( mt_u32& left, u32 const right );
s64 interlocked_or( mt_s64& left, s64 const right );
u64 interlocked_or( mt_u64& left, u64 const right );
s16 interlocked_and( mt_s16& left, s16 const right );
u16 interlocked_and( mt_u16& left, u16 const right );
s32 interlocked_and( mt_s32& left, s32 const right );
u32 interlocked_and( mt_u32& left, u32 const right );
s64 interlocked_and( mt_s64& left, s64 const right );
u64 interlocked_and( mt_u64& left, u64 const right );
s16 interlocked_xor( mt_s16& left, s16 const right );
u16 interlocked_xor( mt_u16& left, u16 const right );
s32 interlocked_xor( mt_s32& left, s32 const right );
u32 interlocked_xor( mt_u32& left, u32 const right );
s64 interlocked_xor( mt_s64& left, s64 const right );
u64 interlocked_xor( mt_u64& left, u64 const right );

u32 interlocked_bit_test_and_set( mt_u32& value, u32 const bit_index );
u32 interlocked_bit_test_and_reset( mt_u32& value, u32 const bit_index );
u64 interlocked_bit_test_and_set( mt_u64& value, u64 const bit_index );
u64 interlocked_bit_test_and_reset( mt_u64& value, u64 const bit_index );

void load_fence( );
void store_fence( );
void full_fence( );

#endif // #ifndef GUARD_CORE_INTERLOCKED_H_INCLUDED