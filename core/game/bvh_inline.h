#ifndef __core_bvh_inline_h_included_
#define __core_bvh_inline_h_included_

template<typename T>
static_bvh<T>::static_bvh( )
{
	ASSERT( aligned( this, 16 ) );
}

template<typename T>
template<typename NodeHeap>
void static_bvh<T>::create( NodeHeap& node_heap, buffer_array<T*> const& objects )
{
	///
}

template<typename T>
void static_bvh<T>::destroy( )
{
	if ( m_root )
		destroy_impl( m_root );
}

template<typename T>
void static_bvh<T>::destroy_impl( node* n )
{
	if ( n->left && n->right )
	{
		destroy_impl( n->left );
		destroy_impl( n->right );
	}
}

template<typename T>
template<typename Callback>
void static_bvh<T>::query_visibility( frustum_aligned const& frustum, Callback callback )
{
	if ( m_root )
		query_visibility_impl( m_root, frustum, callback );
}

template<typename T>
template<typename Callback>
void static_bvh<T>::query_visibility_impl_inside( node* n, Callback callback )
{
	if ( n->left && n->right )
	{
		query_visibility_impl_inside( n->left, callback );
		query_visibility_impl_inside( n->right, callback );
	}
	else
		callback( n->object );
}

template<typename T>
template<typename Callback>
void static_bvh<T>::query_visibility_impl( node* n, frustum_aligned const& frustum, Callback callback )
{
	if ( n->left && n->right )
	{
		intersection result = frustum.test_aabb( n->box );

		switch ( result )
		{
		case inside:
			query_visibility_impl_inside( n->left, callback );
			query_visibility_impl_inside( n->right, callback );
			return;
		case intersect:
			query_visibility_impl( n->left, frustum, callback );
			query_visibility_impl( n->right, frustum, callback );
			return;
		case outside:
			return;
		default:
			UNREACHABLE_CODE
		}
	}
	else
	{
		if ( !frustum.test_aabb_outside( n->box ) )
			callback( n->object );
	}
}

#endif // #ifndef __core_bvh_inline_h_included_