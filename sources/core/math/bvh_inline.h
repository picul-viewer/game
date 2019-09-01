#ifndef GUARD_CORE_BVH_INLINE_H_INCLUDED
#define GUARD_CORE_BVH_INLINE_H_INCLUDED

namespace math {

template<typename Callback>
void bvh::for_each( Callback const& callback ) const
{
	if ( m_root != invalid )
		for_each_impl( callback, m_allocator[m_root] );
}

template<typename Callback>
void bvh::for_each_impl( Callback const& callback, node_data* const node ) const
{
	if ( node->leaf_flag != 0 )
		callback( node->obj );
	else
	{
		for_each_impl( callback, m_allocator[node->left] );
		for_each_impl( callback, m_allocator[node->right] );
	}	
}

template<typename Callback>
void bvh::frustum_test( frustum const& f, Callback const& callback ) const
{
	if ( m_root != invalid )
		frustum_test_impl( f, callback, m_allocator[m_root]	 );
}

template<typename Callback>
void bvh::frustum_test_impl( frustum const& f, Callback const& callback, node_data* const node ) const
{
	aabb box;
	box.set_min_max( node->aabb_min, node->aabb_max );

	if ( node->leaf_flag != 0 )
	{
		if ( !f.test_aabb_outside( box ) )
			callback( node->obj );
	}
	else
	{
		intersection const result = f.test_aabb( box );

		switch ( result )
		{
		case inside:
			for_each_impl( callback, m_allocator[node->left] );
			for_each_impl( callback, m_allocator[node->right] );
			return;
		case intersect:
			frustum_test_impl( f, callback, m_allocator[node->left] );
			frustum_test_impl( f, callback, m_allocator[node->right] );
			return;
		case outside:
			return;
		default:
			UNREACHABLE_CODE
		}
	}
}

} // namespace math

#endif // #ifndef GUARD_CORE_BVH_INLINE_H_INCLUDED