#include <types.h>
#include <lib/allocator.h>
#include <lib/heap.h>
#include <lib/dynamic_heap.h>
#include <system/time.h>
#include <utils/cmd_args.h>
#include <Windows.h>


utils::cmd_arg_bool g_test_std( "test_std", true );
utils::cmd_arg_bool g_test_heap( "test_heap", true );
utils::cmd_arg_bool g_test_dynamic_heap( "test_dynamic_heap", true );

utils::cmd_arg_u32 g_seed( "seed", 1 );
utils::cmd_arg_u32 g_ptrs_count( "ptrs_count", 1024 );
utils::cmd_arg_u32 g_iter_count( "iter_count", 65536 );
utils::cmd_arg_u32 g_min_size( "min_size", 1 );
utils::cmd_arg_u32 g_max_size( "max_size", 64 * Kb );


u32 const rand_max = ( RAND_MAX + 1 ) * ( RAND_MAX + 1 );

u32 random( )
{
	return rand( ) + rand( ) * ( RAND_MAX + 1 );
}

template<typename Allocator>
double perf_test( Allocator& a )
{
	pvoid* const ptrs = stack_allocate( g_ptrs_count * sizeof(pvoid) );
	uptr const size_mult = g_max_size - g_min_size + 1;

	srand( g_seed );

	sys::timer t;
	t.start( );

	for ( u32 i = 0; i < g_ptrs_count; ++i )
	{
		uptr const size = g_min_size + random( ) * size_mult / rand_max;
		ptrs[i] = a.allocate( size );
	}

	for ( u32 i = 0; i < g_iter_count - g_ptrs_count; ++i )
	{
		u32 const index = random( ) % g_ptrs_count;
		uptr const size = g_min_size + random( ) * size_mult / rand_max;
		a.deallocate( ptrs[index] );
		ptrs[index] = a.allocate( size );
	}

	for ( u32 i = 0; i < g_ptrs_count; ++i )
		a.deallocate( ptrs[i] );

	double const time = t.elapsed( );
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

	// Heap allocator.
	if ( g_test_heap )
	{
		uptr const heap_size = 16 * Gb;
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
		u32 const heap_size = 2 * Gb;
		pvoid const heap_memory = virtual_allocator( ).reserve( nullptr, heap_size );

		dynamic_heap h;
		h.create( heap_memory, heap_size );

		double const time = perf_test( h );
		printf( "Dynamic heap allocator time = %3.9lf seconds\n", time );

		virtual_allocator( ).release( heap_memory );
	}
}