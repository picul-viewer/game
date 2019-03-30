#ifndef __core_bvh_h_included_
#define __core_bvh_h_included_

#include <types.h>
#include <lib/buffer_array.h>
#include "aabb.h"
#include "frustum.h"

namespace math {

template<typename T>
class static_bvh
{
public:
	template<typename NodeAllocator>
	void create( NodeAllocator& node_allocator, T** const objects, uptr const objects_size );
	void destroy( );
	
	template<typename Callback>
	void for_each( Callback const& callback ) const;
	template<typename FrustumType, typename Callback>
	void query_visibility( FrustumType const& frustum, Callback const& callback ) const;

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
	node* build( NodeAllocator& node_allocator, T** const first, uptr const count, aabb_aligned* const aabbs, u32 const sorting );
	
	template<typename Callback>
	void for_each_impl( node* const n, Callback const& callback ) const;
	template<typename Callback>
	void query_visibility_impl_inside( node* const n, Callback const& callback ) const;
	template<typename FrustumType, typename Callback>
	void query_visibility_impl( node* const n, FrustumType const& frustum, Callback const& callback ) const;

	void destroy_impl( node* n );

	node* m_root;
};

struct static_bvh_node_size
{
	static const uptr value = sizeof(aabb_aligned) + 2 * sizeof(pointer);
};

} // namespace math

#include "bvh_inline.h"

#endif // #ifndef __core_bvh_h_included_