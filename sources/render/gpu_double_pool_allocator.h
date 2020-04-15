#ifndef GUARD_RENDER_GPU_DOUBLE_POOL_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_GPU_DOUBLE_POOL_ALLOCATOR_H_INCLUDED

#include <types.h>

namespace render {

class gpu_double_pool_allocator
{
public:
	static u32 memory_size( u32 const in_elements_count );

public:
	void create(
		pvoid const in_free_list_memory,
		u32 const in_elements_count,
		u32 const in_left_allocated_count,
		u32 const in_right_allocated_count
	);

	u32 allocate_left( );
	u32 allocate_right( );
	void deallocate_left( u32 const in_index );
	void deallocate_right( u32 const in_index );
	void deallocate( u32 const in_index );

	inline u32 left_last_index( ) { return m_left_last_index; }
	inline u32 right_last_index( ) { return m_right_last_index; }

private:
	u32* m_free_lists;
	u32 m_left_free_count;
	u32 m_right_free_count;
	u32 m_left_last_index;
	u32 m_right_last_index;
	u32 m_elements_count;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_DOUBLE_POOL_ALLOCATOR_H_INCLUDED