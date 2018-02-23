#ifndef __core_bvh_h_included_
#define __core_bvh_h_included_

#include <types.h>
#include <lib/array.h>
#include <game/aabb.h>
#include <game/frustum.h>

template<typename T>
class static_bvh
{
public:
	static_bvh( );

	template<typename NodeAllocator>
	void create( NodeAllocator& node_allocator, buffer_array<T*>& objects );
	void destroy( );

	template<typename Callback>
	void query_visibility( frustum_aligned const& frustum, Callback callback );

protected:
	struct node
	{
		aabb_aligned box;
		union
		{
			node* left;
			T* object;
		};
		node* right;
	};
	
	template<typename NodeAllocator>
	node* build( NodeAllocator& node_allocator, T** first, uptr count, aabb_aligned* aabbs, u32 sorting );

	template<typename Callback>
	void query_visibility_impl_inside( node* n, Callback callback );
	template<typename Callback>
	void query_visibility_impl( node* n, frustum_aligned const& frustum, Callback callback );

	void destroy_impl( node* n );

	node* m_root;
};

struct static_bvh_node_size
{
	static const uptr value = sizeof(aabb_aligned) + 2 * sizeof(pointer);
};

#include "bvh_inline.h"

#endif // #ifndef __core_bvh_h_included_