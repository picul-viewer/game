#ifndef __core_bvh_inline_h_included_
#define __core_bvh_inline_h_included_

#include <lib/memory.h>
#include <lib/algorithms.h>

namespace math {

template<typename T>
static inline float get_linear_sah( T* const objects, uptr const count, aabb_aligned* const aabbs, uptr& best_division )
{
	aabb_aligned current_box;
	float sah = FLT_MAX;

	current_box.set_min_max( _mm_set1_ps( FLT_MAX ), _mm_set1_ps( FLT_MIN ) );

	for ( uptr i = 0; i < count - 1; ++i )
	{
		current_box.extrude( objects[i]->get_aabb( ) );
		aabbs[i] = current_box;
	}

	current_box.set_min_max( _mm_set1_ps( FLT_MAX ), _mm_set1_ps( FLT_MIN ) );

	float const all_count = (float)count;
	float left_count = (float)( count - 1 );

	uptr best_left_count;

	for ( uptr i = count - 1; i > 0; --i, --left_count )
	{
		current_box.extrude( objects[i]->get_aabb( ) );
		float const local_sah = aabbs[i].surface_area( ) * left_count + current_box.surface_area( ) * ( all_count - left_count );
		if ( local_sah < sah )
		{
			sah = local_sah;
			best_left_count = i;
		}
	}

	best_division = best_left_count;
	return sah;
}

template<typename T>
template<typename NodeAllocator>
typename static_bvh<T>::node* static_bvh<T>::build( NodeAllocator& node_allocator, T** const first, uptr const count, aabb_aligned* const aabbs, u32 const sorting )
{
	node* n = node_allocator.allocate( sizeof(node) );

	if ( count == 1 )
	{
		n->box = first[0]->get_aabb( );
		n->object = first[0];
		n->right = nullptr;
		return n;
	}
	
	if ( count == 2 )
	{
		node* l = node_allocator.allocate( sizeof(node) );
		node* r = node_allocator.allocate( sizeof(node) );

		l->box = first[0]->get_aabb( );
		r->box = first[1]->get_aabb( );
		l->object = first[0];
		r->object = first[1];
		l->right = nullptr;
		r->right = nullptr;

		n->box = l->box;
		n->box.extrude( r->box );
		n->left = l;
		n->right = r;

		return n;
	}
	
	uptr buffer_size = count * sizeof(T*);

	T** objects[3];

	u32 sorting1 = ( sorting + 1 ) % 3;
	u32 sorting2 = ( sorting + 2 ) % 3;

	{
		// Don't sort more then we need
		objects[sorting] = first;

		objects[sorting1] = aligned_std_allocator<16>( ).allocate( buffer_size );
		objects[sorting2] = aligned_std_allocator<16>( ).allocate( buffer_size );
		
		memory::copy( objects[sorting1], first, buffer_size );
		memory::copy( objects[sorting2], first, buffer_size );
		
		lib::sort( objects[sorting1], objects[sorting1] + count, [sorting1]( T* l, T* r )
		{
			return l->get_aabb( ).get_box( ).get_max( ).data[sorting1] < r->get_aabb( ).get_box( ).get_max( ).data[sorting1];
		} );
		
		lib::sort( objects[sorting2], objects[sorting2] + count, [sorting2]( T* l, T* r )
		{
			return l->get_aabb( ).get_box( ).get_max( ).data[sorting2] < r->get_aabb( ).get_box( ).get_max( ).data[sorting2];
		} );
	}

	uptr found_divisions[3];
	
	float sah[3];
	
	sah[0] = get_linear_sah( objects[0], count, aabbs, found_divisions[0] );

	// Now can get node AABB
	aabb_aligned node_box = aabbs[count - 2];
	node_box.extrude( objects[0][count - 1]->get_aabb( ) );
	n->box = node_box;

	sah[1] = get_linear_sah( objects[1], count, aabbs, found_divisions[1] );
	sah[2] = get_linear_sah( objects[2], count, aabbs, found_divisions[2] );

	if ( ( sah[0] > sah[1] ) && ( sah[0] > sah[2] ) )
	{
		n->left = build( node_allocator, objects[0], found_divisions[0], aabbs, 0 );
		n->right = build( node_allocator, objects[0] + found_divisions[0], count - found_divisions[0], aabbs, 0 );
	}
	else if ( ( sah[1] > sah[0] ) && ( sah[1] > sah[2] ) )
	{
		n->left = build( node_allocator, objects[1], found_divisions[1], aabbs, 1 );
		n->right = build( node_allocator, objects[1] + found_divisions[1], count - found_divisions[1], aabbs, 1 );
	}
	else
	{
		n->left = build( node_allocator, objects[2], found_divisions[2], aabbs, 2 );
		n->right = build( node_allocator, objects[2] + found_divisions[2], count - found_divisions[2], aabbs, 2 );
	}

	aligned_std_allocator<16>( ).deallocate( objects[sorting1] );
	aligned_std_allocator<16>( ).deallocate( objects[sorting2] );

	return n;
}

template<typename T>
template<typename NodeAllocator>
void static_bvh<T>::create( NodeAllocator& node_allocator, T** const objects, uptr const objects_size )
{
	if ( objects_size == 0 )
		return;

	if ( objects_size == 1 )
	{
		m_root = node_allocator.allocate( sizeof(node) );
		m_root->box = objects[0]->get_aabb( );
		m_root->object = objects[0];
		m_root->right = nullptr;
		return;
	}
	
	if ( objects_size == 2 )
	{
		m_root = node_allocator.allocate( sizeof(node) );
		node* l = node_allocator.allocate( sizeof(node) );
		node* r = node_allocator.allocate( sizeof(node) );

		l->box = objects[0]->get_aabb( );
		r->box = objects[1]->get_aabb( );
		l->object = objects[0];
		r->object = objects[1];
		l->right = nullptr;
		r->right = nullptr;

		m_root->box = l->box;
		m_root->box.extrude( r->box );
		m_root->left = l;
		m_root->right = r;

		return;
	}
	
	aabb_aligned* aabbs = aligned_std_allocator<16>( ).allocate( objects_size * sizeof(aabb_aligned) );

	// Pre-sort by max.x -> sorting == 0
	lib::sort( objects, objects + objects_size, []( T* l, T* r )
	{
		return l->get_aabb( ).get_box( ).get_max( ).x < r->get_aabb( ).get_box( ).get_max( ).x;
	} );

	m_root = build( node_allocator, objects, objects_size, aabbs, 0 );

	aligned_std_allocator<16>( ).deallocate( aabbs );
}

template<typename T>
void static_bvh<T>::destroy( )
{
	if ( m_root )
		destroy_impl( m_root );
}

template<typename T>
void static_bvh<T>::destroy_impl( node* const n )
{
	if ( n->left && n->right )
	{
		destroy_impl( n->left );
		destroy_impl( n->right );
	}
}

template<typename T>
template<typename Callback>
void static_bvh<T>::for_each( Callback const& callback ) const
{
	if ( m_root )
		for_each_impl( m_root, callback );
}

template<typename T>
template<typename Callback>
void static_bvh<T>::for_each_impl( node* const n, Callback const& callback ) const
{
	if ( n->right )
	{
		for_each_impl( n->left, callback );
		for_each_impl( n->right, callback );
	}
	else
		callback( n->object );
}

template<typename T>
template<typename FrustumType, typename Callback>
void static_bvh<T>::query_visibility( FrustumType const& frustum, Callback const& callback ) const
{
	if ( m_root )
		query_visibility_impl( m_root, frustum, callback );
}

template<typename T>
template<typename Callback>
void static_bvh<T>::query_visibility_impl_inside( node* const n, Callback const& callback ) const
{
	if ( n->right )
	{
		query_visibility_impl_inside( n->left, callback );
		query_visibility_impl_inside( n->right, callback );
	}
	else
		callback( n->object );
}

template<typename T>
template<typename FrustumType, typename Callback>
void static_bvh<T>::query_visibility_impl( node* const n, FrustumType const& frustum, Callback const& callback ) const
{
	if ( n->right )
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

} // namespace math

#endif // #ifndef __core_bvh_inline_h_included_