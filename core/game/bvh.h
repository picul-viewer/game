#ifndef __core_bvh_h_included_
#define __core_bvh_h_included_

#include <core/types.h>
#include <core/std.h>

template<typename T>
class static_bvh
{
public:
	static_bvh( );

	template<typename NodeHeap>
	void create( NodeHeap& node_heap, buffer_array<T*> const& objects );
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
	
	template<typename Callback>
	void query_visibility_impl_inside( node* n, Callback callback );
	template<typename Callback>
	void query_visibility_impl( node* n, frustum_aligned const& frustum, Callback callback );

	void destroy_impl( node* n );

	node* m_root;
};

#include "bvh_inline.h"

#endif // #ifndef __core_bvh_h_included_