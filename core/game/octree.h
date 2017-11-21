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

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
class octree
{
public:
	octree( );

	void create( aabb_aligned const& box, u32 max_depth = -1 );
	void create( aabb_aligned const& box, float max_node_radius );
	void destroy( );

	void insert( T* object );
	void move( T* object );
	void remove( T* object );

protected:
	struct node
	{
		node* nodes[8] = { };
		NodeObjectContainer<T> objects;
	};

	typename node* new_node( );
	void calculate_bounds( aabb_aligned const& box );
	
	math::float4 m_box_center;
	math::float4 m_box_half_radius;
	NodeHeap<node> m_nodes;
	node* m_root;
	u32 max_depth;
};

#include "octree_inline.h"

#endif // #ifndef __core_octree_h_included_