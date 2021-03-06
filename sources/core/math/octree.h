#ifndef GUARD_CORE_OCTREE_H_INCLUDED
#define GUARD_CORE_OCTREE_H_INCLUDED

#include <types.h>

#include "vector.h"
#include "aabb.h"
#include "frustum.h"

namespace math {

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
class octree
{
public:
	octree( ) = default;

	void create( NodeAllocator& node_allocator, aabb_aligned const& box, u32 max_depth = (u32)-1 );
	void create( NodeAllocator& node_allocator, aabb_aligned const& box, float node_min_radius );
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

	float4 m_box_center;
	float4 m_box_half_radius;
	NodeAllocator* m_nodes;
	node* m_root;
	u32 max_depth;
};

template<typename NodeObjectContainer>
struct octree_node_size
{
	static const uptr value = 8 * sizeof(pointer) + sizeof(NodeObjectContainer);
};

} // namespace math

#include "octree_inline.h"

#endif // #ifndef GUARD_CORE_OCTREE_H_INCLUDED