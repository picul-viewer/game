#define _HAS_EXCEPTIONS 0
#include <types.h>
#include <random>
#include <lib/allocator.h>
#include <lib/heap.h>
#include <lib/dynamic_heap.h>
#include <lib/mt_heap.h>
#include <system/interlocked.h>
#include <system/thread.h>
#include <system/time.h>
#include <utils/cmd_args.h>
#include <Windows.h>


utils::cmd_arg_bool g_test_std( "test_std", true );
utils::cmd_arg_bool g_test_mt_std( "test_mt_std", true );
utils::cmd_arg_bool g_test_heap( "test_heap", true );
utils::cmd_arg_bool g_test_dynamic_heap( "test_dynamic_heap", true );
utils::cmd_arg_bool g_test_mt_heap_st( "test_mt_heap_st", true );
utils::cmd_arg_bool g_test_mt_heap_mt( "test_mt_heap_mt", true );

utils::cmd_arg_u32 g_seed( "seed", 1 );
utils::cmd_arg_u32 g_ptrs_count( "ptrs_count", 1024 );
utils::cmd_arg_u32 g_iter_count( "iter_count", 65536 );
utils::cmd_arg_u32 g_thread_count( "thread_count", 6 );
utils::cmd_arg_u32 g_min_size( "min_size", 1 );
utils::cmd_arg_u32 g_max_size( "max_size", 64 * Kb );


template<typename Allocator>
double perf_test( Allocator& a )
{
	pvoid* const ptrs = stack_allocate( g_ptrs_count * sizeof(pvoid) );
	memset( ptrs, 0, g_ptrs_count * sizeof(pvoid) );
	uptr const size_mult = g_max_size - g_min_size + 1;

	std::default_random_engine g;
	g.seed( g_seed );

	sys::timer t;
	t.start( );

	for ( u32 i = 0; i < g_iter_count; ++i )
	{
		u32 const index = g( ) % g_ptrs_count;

		if ( ptrs[index] )
		{
			a.deallocate( ptrs[index] );
			ptrs[index] = nullptr;
		}
		else
		{
			uptr const size = g_min_size + g( ) * size_mult / g.max( );
			ptrs[index] = a.allocate( size );
		}
	}

	for ( u32 i = 0; i < g_ptrs_count; ++i )
		if ( ptrs[i] )
			a.deallocate( ptrs[i] );

	double const time = t.elapsed( );
	return time;
}

struct mt_info
{
	mt_u64* ptrs;
	pvoid allocator;
	double time;
	mt_u32* finish_counter;
	u32 thread_index;
	u32 seed;
};

template<typename Allocator>
void mt_perf_test_thread_func( void* arg )
{
	mt_info& info = *(mt_info*)arg;
	mt_u64* const ptrs = info.ptrs;
	Allocator& a = *(Allocator*)info.allocator;
	uptr const size_mult = g_max_size - g_min_size + 1;

	std::default_random_engine g;
	g.seed( info.seed );

	sys::timer t;
	t.start( );

	u32 const iter_count = ( g_iter_count + info.thread_index ) / g_thread_count;

	for ( u32 i = 0; i < iter_count; ++i )
	{
		u64 ptr;
		u32 index;

		do
		{
			do
			{
				index = g( ) % g_ptrs_count;
				ptr = ptrs[index];
			}
			while ( ptr == (u64)-1 );
		}
		while ( interlocked_compare_exchange( ptrs[index], (u64)-1, ptr ) != ptr );

		if ( ptr )
		{
			a.deallocate( (pvoid)ptr );
			ptrs[index] = 0;
		}
		else
		{
			uptr const size = g_min_size + g( ) * size_mult / g.max( );
			ptrs[index] = a.allocate( size );
		}
	}

	if ( interlocked_dec( *info.finish_counter ) == 0 )
	{
		for ( u32 i = 0; i < g_ptrs_count; ++i )
			if ( ptrs[i] )
				a.deallocate( (pvoid)ptrs[i] );
	}

	info.time = t.elapsed( );
}

template<typename Allocator>
double mt_perf_test( Allocator& a )
{
	mt_u64* const ptrs = stack_allocate( g_ptrs_count * sizeof(mt_u64) );
	memset( (pvoid)ptrs, 0, g_ptrs_count * sizeof(mt_u64) );

	mt_info* thread_info = stack_allocate( g_thread_count * sizeof(mt_info) );
	sys::thread* threads = stack_allocate( g_thread_count * sizeof(sys::thread) );
	mt_u32 finish_counter = g_thread_count;

	for ( u32 i = 0; i < g_thread_count; ++i )
	{
		thread_info[i].ptrs = ptrs;
		thread_info[i].allocator = &a;
		thread_info[i].finish_counter = &finish_counter;
		thread_info[i].thread_index = i;
		thread_info[i].seed = g_seed + i * 13;
	}

	for ( u32 i = 0; i < g_thread_count; ++i )
	{
		threads[i].create( sys::thread::func_helper<&mt_perf_test_thread_func<Allocator>>, 4 * Mb, &thread_info[i] );
		threads[i].force_processor_index( i );
	}

	double time = 0.0;
	for ( u32 i = 0; i < g_thread_count; ++i )
	{
		threads[i].destroy( );
		time += thread_info[i].time;
	}

	return time;
}

int main( )
{
	// Std allocator.
	if ( g_test_std )
	{
		double const time = perf_test( std_allocator( ) );
		printf( "Std allocator time = %3.9lf seconds\n", time );
	}

	// Std allocator (multithreaded).
	if ( g_test_mt_std )
	{
		double const time = mt_perf_test( std_allocator( ) );
		printf( "Std allocator multithreaded time = %3.9lf seconds\n", time );
	}

	// Heap allocator.
	if ( g_test_heap )
	{
		uptr const heap_size = 8 * Gb;
		pvoid const heap_memory = virtual_allocator( ).allocate( heap_size );

		heap h;
		h.create( heap_memory, heap_size );

		double const time = perf_test( h );
		printf( "Heap allocator time = %3.9lf seconds\n", time );

		virtual_allocator( ).deallocate( heap_memory );
	}

	// Dynamic heap allocator.
	if ( g_test_dynamic_heap )
	{
		uptr const heap_size = 8 * Gb;
		pvoid const heap_memory = virtual_allocator( ).reserve( nullptr, heap_size );

		dynamic_heap h;
		h.create( heap_memory, heap_size );

		double const time = perf_test( h );
		printf( "Dynamic heap allocator time = %3.9lf seconds\n", time );

		virtual_allocator( ).release( heap_memory );
	}

	uptr const heap_size = 8 * Gb;
	pvoid const heap_memory = virtual_allocator( ).allocate( heap_size );

	// Multithreaded heap allocator ( singlethreaded test ).
	if ( g_test_mt_heap_st )
	{
		mt_heap h;
		h.create( heap_memory, heap_size );

		double const time = perf_test( h );
		printf( "MT heap allocator (ST) time = %3.9lf seconds\n", time );
	}

	// Multithreaded heap allocator ( multithreaded test ).
	if ( g_test_mt_heap_mt )
	{
		mt_heap h;
		h.create( heap_memory, heap_size );

		double const time = mt_perf_test( h );
		printf( "MT heap allocator (MT) time = %3.9lf seconds\n", time );
	}

	virtual_allocator( ).deallocate( heap_memory );
}