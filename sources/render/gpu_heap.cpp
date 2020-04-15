#include "gpu_heap.h"
#include <lib/algorithms.h>
#include <math/math_common.h>
#include "dx.h"

namespace render {

void gpu_heap::create( u64 const in_heap_size, pointer const in_free_blocks_memory, uptr const in_max_free_blocks )
{
	m_free_blocks.create( in_free_blocks_memory, in_max_free_blocks );
	m_free_blocks.push_back( { 0, in_heap_size } );
}

void gpu_heap::destroy( )
{ }

u64 gpu_heap::allocate( D3D12_RESOURCE_ALLOCATION_INFO const& in_allocation_info )
{
	block_info* block;
	
	for ( block = m_free_blocks.begin( ); block != m_free_blocks.end( ); ++block )
	{
		if ( block->offset + block->size >= math::align_up( block->offset, in_allocation_info.Alignment ) + in_allocation_info.SizeInBytes )
			break;
	}

	ASSERT( block != m_free_blocks.end( ) );

	for ( block_info* i = block + 1; block != m_free_blocks.end( ); ++i )
	{
		if ( ( i->offset + i->size >= math::align_up( i->offset, in_allocation_info.Alignment ) + in_allocation_info.SizeInBytes ) && ( i->size < block->size ) )
			block = i;
	}

	return 0;
}

void gpu_heap::deallocate( u64 const in_offset, u64 const in_size )
{

}

} // namespace render