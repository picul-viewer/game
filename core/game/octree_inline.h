#ifndef __core_octree_inline_h_included_
#define __core_octree_inline_h_included_

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::octree( )
{
	ASSERT( aligned( this, 16 ) );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
typename octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::node* octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::new_node( )
{
	node* n = m_nodes->allocate( sizeof(node) );
	memset( n, 0, sizeof(node) );
	return n;
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::calculate_bounds( aabb_aligned const& box )
{
	__m128 l = box.min.data;
	__m128 h = box.max.data;
	
	__m128 sum = _mm_add_ps( l, h );

	__m128 c = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );
	__m128 r = _mm_sub_ps( h, c );
	__m128 hr = _mm_mul_ps( r, _mm_set1_ps( 0.5f ) );

	_mm_store_ps( m_box_center.data, c );
	_mm_store_ps( m_box_half_radius.data, hr );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::create( NodeAllocator& node_allocator, aabb_aligned const& box, u32 max_depth )
{
	this->m_nodes = &node_allocator;
	calculate_bounds( box );
	m_root = new_node( );
	this->max_depth = max_depth;
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::create( NodeAllocator& node_allocator, aabb_aligned const& box, float node_min_radius )
{
	this->m_nodes = &node_allocator;
	calculate_bounds( box );
	m_root = new_node( );

	math::float3 const& radius_scaled = m_box_half_radius * ( 1.0f / node_min_radius );
	math::float3 const depth( log2f( radius_scaled.x ), log2f( radius_scaled.y ), log2f( radius_scaled.z ) );
	max_depth = (u32)(int)math::max( math::max( depth.x, depth.y ), depth.z );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::destroy_impl( node* n )
{
	for ( u32 i = 0; i < 8; ++i )
		if ( n->nodes[i] )
			destroy_impl( n->nodes[i] );

	m_nodes->deallocate( n );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::destroy( )
{
	if ( m_root )
		destroy_impl( m_root );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::insert( T* object )
{
	node* n = m_root;

	aabb_aligned const& obj_box = object->get_aabb( );
	
	__m128 obj_box_min = obj_box.min.data;
	__m128 obj_box_max = obj_box.max.data;
	
	__m128 tree_box_center = _mm_load_ps( m_box_center.data );
	__m128 tree_box_radius = _mm_load_ps( m_box_half_radius.data );

	__m128 obj_box_min_diff = _mm_sub_ps( tree_box_center, obj_box_min );
	__m128 obj_box_max_diff = _mm_sub_ps( tree_box_center, obj_box_max );

	__m128 obj_box_diff_test = _mm_xor_ps( obj_box_min_diff, obj_box_max_diff );
	__m128 msb_mask = _mm_castsi128_ps( _mm_setr_epi32( INT_MIN, INT_MIN, INT_MIN, 0 ) );
	
	for ( u32 i = 0; _mm_testz_ps( obj_box_diff_test, msb_mask ) && ( i < max_depth ); ++i )
	{
		u32 octant = _mm_movemask_ps( obj_box_min_diff );

		if ( n->nodes[octant] )
			n = n->nodes[octant];
		else
			n = n->nodes[octant] = new_node( );

		tree_box_radius = _mm_mul_ps( tree_box_radius, _mm_set1_ps( 0.5f ) );

		__m128 signed_radius = _mm_or_ps( tree_box_radius, _mm_and_ps( obj_box_min_diff, msb_mask ) );

		obj_box_min_diff = _mm_add_ps( obj_box_min_diff, signed_radius );
		obj_box_max_diff = _mm_add_ps( obj_box_max_diff, signed_radius );
	}

	n->objects.insert( object );
	object->*NodePtr = n;
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::move( T* object )
{
	remove( object );
	insert( object );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::remove( T* object )
{
	node* n = object->*NodePtr;
	object->*NodePtr = nullptr;

	n->objects.remove( object );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
template<typename Callback>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::query_visibility( frustum_aligned const& frustum, Callback callback )
{
	ASSERT( aligned( this, 16 ) );

	if ( !m_root ) return;

	__m128 center		= _mm_load_ps( m_box_center.data );
	__m128 half_radius	= _mm_load_ps( m_box_half_radius.data );

	query_visibility_impl( m_root, center, half_radius, frustum, callback );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
template<typename Callback>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::query_visibility_impl_inside( node* n, Callback callback )
{
	for ( u32 i = 0; i < 8; ++i )
		if ( n->nodes[i] )
			query_visibility_impl_inside( n->nodes[i], callback );

	n->objects.for_each( [callback]( T* current )
	{
		callback( current );
	} );
}

template<typename T, typename NodeObjectContainer, typename NodeAllocator, pointer (T::*NodePtr)>
template<typename Callback>
void octree<T, NodeObjectContainer, NodeAllocator, NodePtr>::query_visibility_impl( node* n, __m128 const& node_center, __m128 const& node_half_radius, frustum_aligned const& frustum, Callback callback )
{
	aabb_aligned node_aabb;
	node_aabb.set_center_radius( node_center, _mm_add_ps( node_half_radius, node_half_radius ) );

	switch ( frustum.test_aabb( node_aabb ) )
	{
	case inside:
		query_visibility_impl_inside( n, callback );
		return;
	case intersect:
		break;
	case outside:
		return;
	default:
		UNREACHABLE_CODE
	}
	
	__m128 node_neg_half_radius = _mm_xor_ps( node_half_radius, _mm_castsi128_ps( _mm_setr_epi32( 0x80000000, 0x80000000, 0x80000000, 0x0 ) ) );

	__m128 octant_half_radius = _mm_mul_ps( node_half_radius, _mm_set1_ps( 0.5f ) );

	// Test all octants
	{
		if ( n->nodes[0] )
		{
			__m128 octant_center = _mm_sub_ps( node_center, node_half_radius );
			query_visibility_impl( n->nodes[0], octant_center, octant_half_radius, frustum, callback );
		}

		if ( n->nodes[1] )
		{
			__m128 octant_center = _mm_add_ps( node_center, _mm_blend_ps( node_neg_half_radius, node_half_radius, 1 ) );
			query_visibility_impl( n->nodes[1], octant_center, octant_half_radius, frustum, callback );
		}

		if ( n->nodes[2] )
		{
			__m128 octant_center = _mm_add_ps( node_center, _mm_blend_ps( node_neg_half_radius, node_half_radius, 2 ) );
			query_visibility_impl( n->nodes[2], octant_center, octant_half_radius, frustum, callback );
		}

		if ( n->nodes[3] )
		{
			__m128 octant_center = _mm_add_ps( node_center, _mm_blend_ps( node_neg_half_radius, node_half_radius, 3 ) );
			query_visibility_impl( n->nodes[3], octant_center, octant_half_radius, frustum, callback );
		}

		if ( n->nodes[4] )
		{
			__m128 octant_center = _mm_add_ps( node_center, _mm_blend_ps( node_neg_half_radius, node_half_radius, 4 ) );
			query_visibility_impl( n->nodes[4], octant_center, octant_half_radius, frustum, callback );
		}

		if ( n->nodes[5] )
		{
			__m128 octant_center = _mm_add_ps( node_center, _mm_blend_ps( node_neg_half_radius, node_half_radius, 5 ) );
			query_visibility_impl( n->nodes[5], octant_center, octant_half_radius, frustum, callback );
		}

		if ( n->nodes[6] )
		{
			__m128 octant_center = _mm_add_ps( node_center, _mm_blend_ps( node_neg_half_radius, node_half_radius, 6 ) );
			query_visibility_impl( n->nodes[6], octant_center, octant_half_radius, frustum, callback );
		}
		
		if ( n->nodes[7] )
		{
			__m128 octant_center = _mm_add_ps( node_center, node_half_radius );
			query_visibility_impl( n->nodes[7], octant_center, octant_half_radius, frustum, callback );
		}
	}

	n->objects.for_each( [&frustum, callback]( T* current )
	{
		if ( !frustum.test_aabb_outside( current->get_aabb( ) ) )
			callback( current );
	} );
}

#endif // #ifndef __core_octree_inline_h_included_