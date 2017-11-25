#ifndef __core_octree_h_included_
#define __core_octree_h_included_

#include <core/types.h>
#include <core/structs.h>

/*template<typename T, typename Allocator = mem_allocator>
class static_octree
{
public:
	void create( buffer_array<T> const& objects );

protected:
	struct node
	{
		u32 start_node_index, end_node_index;
		u32 start_object_index, end_object_index;
	};

	buffer_array<node> m_nodes;
	buffer_array<T> m_objects;
};*/

template<typename T, typename NodeObjectContainer, typename NodeHeap, pointer (T::*NodePtr)>
class octree
{
public:
	octree( );

	void create( NodeHeap& node_heap, aabb_aligned const& box, u32 max_depth = -1 );
	void create( NodeHeap& node_heap, aabb_aligned const& box, float node_min_radius );
	void destroy( );

	void insert( T* object );
	void move( T* object );
	void remove( T* object );

	template<typename Callback>
	void query_visibility( frustum_aligned const& frustum, Callback callback );

protected:
	struct node
	{
		node* nodes[8] = { };
		NodeObjectContainer objects;
	};

	typename node* new_node( );
	void calculate_bounds( aabb_aligned const& box );
	
	template<typename Callback>
	void query_visibility_impl_inside( node* n, Callback callback );
	template<typename Callback>
	void query_visibility_impl( node* n, __m128 const& node_center, __m128 const& node_half_radius, frustum_aligned const& frustum, Callback callback );

	void destroy_impl( node* n );

	math::float4 m_box_center;
	math::float4 m_box_half_radius;
	NodeHeap* m_nodes;
	node* m_root;
	u32 max_depth;
};

template<typename NodeObjectContainer>
struct octree_node_size
{
	static const uptr value = 8 * sizeof(pointer) + sizeof(NodeObjectContainer);
};

#include "octree_inline.h"

#endif // #ifndef __core_octree_h_included_