#ifndef __core_octree_inline_h_included_
#define __core_octree_inline_h_included_

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
octree<T, NodeObjectContainer, NodeHeap, NodePtr>::octree( )
{
	ASSERT( aligned( this, 16 ) );
}

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
typename octree<T, NodeObjectContainer, NodeHeap, NodePtr>::node* octree<T, NodeObjectContainer, NodeHeap, NodePtr>::new_node( )
{
	node* n = m_nodes.allocate( );
	memset( n, 0, sizeof(node) );
	return n;
}

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeHeap, NodePtr>::create( aabb_aligned const& box )
{
	__m128 l = _mm_load_ps( box.min.data );
	__m128 h = _mm_load_ps( box.max.data );
	
	__m128 sum = _mm_add_ps( l, h );

	__m128 c = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );
	__m128 r = _mm_sub_ps( h, c );
	__m128 hr = _mm_mul_ps( r, _mm_set1_ps( 0.5f ) );

	_mm_store_ps( m_box_center.data, c );
	_mm_store_ps( m_box_half_radius.data, hr );

	m_root = new_node( );
}

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeHeap, NodePtr>::destroy( )
{
	m_nodes.destroy( );
}

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeHeap, NodePtr>::insert( T* object )
{
	node* n = m_root;

	aabb_aligned const& obj_box = object->get_aabb( );
	
	__m128 obj_box_min = _mm_load_ps( object_box.min.data );
	__m128 obj_box_max = _mm_load_ps( object_box.max.data );
	
	__m128 tree_box_center = _mm_load_ps( m_box_center.min.data );
	__m128 tree_box_radius = _mm_load_ps( m_box_half_radius.max.data );

	__m128 obj_box_min_diff = _mm_sub_ps( tree_box_center, obj_box_min );
	__m128 obj_box_max_diff = _mm_sub_ps( tree_box_center, obj_box_max );

	__m128 obj_box_diff_test = _mm_xor_ps( obj_box_min_diff, obj_box_max_diff );
	__m128 msb_mask = _mm_castsi128_ps( _mm_setr_epi32( INT_MIN, INT_MIN, INT_MIN, 0 ) );
	
	while ( _mm_testz_ps( obj_box_diff_test, msb_mask ) )
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

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeHeap, NodePtr>::move( T* object )
{
	remove( object );
	insert( object );
}

template<typename T, template<typename T> typename NodeObjectContainer, template<typename T> typename NodeHeap, pointer* (T::*NodePtr)>
void octree<T, NodeObjectContainer, NodeHeap, NodePtr>::remove( T* object )
{
	node* n = object->*NodePtr;
	object->*NodePtr = nullptr;

	n->objects->remove( object );
}

#endif // #ifndef __core_octree_inline_h_included_