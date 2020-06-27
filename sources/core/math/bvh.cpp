#include "bvh.h"
#include <lib/algorithms.h>
#include <lib/linear_queue.h>
#include <lib/memory.h>

namespace math {

void bvh::create( pointer const memory, uptr const memory_size )
{
	static_assert( node_data_size == sizeof(node_data), "incorrect node data size" );

	m_allocator.create( memory, memory_size );
	m_root = invalid;
}

void bvh::destroy( )
{ }

void bvh::build( aabb const* const aabbs, object_handle const* const handles, uptr const count )
{
	if ( count == 0 )
	{
		m_root = invalid;
		return;
	}

	uptr const memory_size = count * ( sizeof(node_data*) * 3 + sizeof(aabb) );
	pointer const memory = virtual_allocator( ).allocate( memory_size );

	node_data** const nodes_0 = memory;
	node_data** const nodes_1 = (pointer)nodes_0 + count * sizeof(node_data*);
	node_data** const nodes_2 = (pointer)nodes_1 + count * sizeof(node_data*);

	aabb* const aabb_memory = (pointer)nodes_2 + count * sizeof(node_data*);

	for ( uptr i = 0; i < count; ++i )
	{
		nodes_0[i] = m_allocator.allocate( );

		nodes_0[i]->aabb_min = aabbs[i].get_min( );
		nodes_0[i]->aabb_max = aabbs[i].get_max( );
		nodes_0[i]->obj = handles[i];
		nodes_0[i]->leaf_flag = 1;
	}

	lib::sort( nodes_0, nodes_0 + count, []( node_data* const l, node_data* const r )
	{
		return l->aabb_max.x < r->aabb_max.x;
	} );

	m_root = build_impl( nodes_0, nodes_1, nodes_2, aabb_memory, count, 0 );

	virtual_allocator( ).deallocate( memory );
}

bvh::node_handle bvh::build_impl(
	node_data** const nodes_0,
	node_data** const nodes_1,
	node_data** const nodes_2,
	aabb* const aabb_memory,
	uptr const count,
	u32 const sorting_component
)
{
	if ( count == 1 )
		return (node_handle)m_allocator.index_of( nodes_0[0] );
	
	node_data* p = m_allocator.allocate( );
	p->leaf_flag = 0;
	node_handle const p_handle = (node_handle)m_allocator.index_of( p );

	if ( count == 2 )
	{
		p->aabb_min = min( nodes_0[0]->aabb_min, nodes_0[1]->aabb_min );
		p->aabb_max = max( nodes_0[0]->aabb_max, nodes_0[1]->aabb_max );
		p->left = (node_handle)m_allocator.index_of( nodes_0[0] );
		p->right = (node_handle)m_allocator.index_of( nodes_0[1] );

		nodes_0[0]->parent = p_handle;
		nodes_0[1]->parent = p_handle;

		return p_handle;
	}

	memory::copy( nodes_1, nodes_0, count * sizeof(node_data*) );
	memory::copy( nodes_2, nodes_0, count * sizeof(node_data*) );

	u32 const sorting_component_0 = ( sorting_component + 1 ) % 3;
	u32 const sorting_component_1 = ( sorting_component + 2 ) % 3;

	lib::sort( nodes_1, nodes_1 + count, [sorting_component_0]( node_data* const l, node_data* const r )
	{
		return l->aabb_max.data[sorting_component_0] < r->aabb_max.data[sorting_component_0];
	} );

	lib::sort( nodes_2, nodes_2 + count, [sorting_component_1]( node_data* const l, node_data* const r )
	{
		return l->aabb_max.data[sorting_component_1] < r->aabb_max.data[sorting_component_1];
	} );

	uptr sah_divisions[3];
	float sah_sums[3];

	sah_divisions[0] = divide_by_sah( nodes_0, aabb_memory, count, sah_sums[0] );
	sah_divisions[1] = divide_by_sah( nodes_1, aabb_memory, count, sah_sums[1] );
	sah_divisions[2] = divide_by_sah( nodes_2, aabb_memory, count, sah_sums[2] );

	p->aabb_min = aabb_memory[count - 1].get_min( );
	p->aabb_max = aabb_memory[count - 1].get_max( );

	u32 best_sah_index = 0;
	best_sah_index = ( sah_sums[1] < sah_sums[best_sah_index] ) ? 1 : best_sah_index;
	best_sah_index = ( sah_sums[2] < sah_sums[best_sah_index] ) ? 2 : best_sah_index;

	node_data** const memory[3] = {
		nodes_0,
		nodes_1,
		nodes_2
	};

	p->left = build_impl(
		memory[best_sah_index],
		memory[( best_sah_index + 1 ) % 3],
		memory[( best_sah_index + 2 ) % 3],
		aabb_memory,
		sah_divisions[best_sah_index],
		( sorting_component + best_sah_index ) % 3
	);

	p->right = build_impl(
		memory[best_sah_index] + sah_divisions[best_sah_index],
		memory[( best_sah_index + 1 ) % 3] + sah_divisions[best_sah_index],
		memory[( best_sah_index + 2 ) % 3] + sah_divisions[best_sah_index],
		aabb_memory,
		count - sah_divisions[best_sah_index],
		( sorting_component + best_sah_index ) % 3
	);

	node_data* const l = m_allocator[p->left];
	node_data* const r = m_allocator[p->right];

	l->parent = p_handle;
	r->parent = p_handle;

	return p_handle;
}

uptr bvh::divide_by_sah(
	node_data** const nodes,
	aabb* const aabbs,
	uptr const count,
	float& sah_sum
)
{
	aabb current_box;

	current_box.invalidate( );

	for ( uptr i = 0; i < count; ++i )
	{
		aabb box;
		box.set_min_max( nodes[i]->aabb_min, nodes[i]->aabb_max );

		current_box.extrude( box );
		aabbs[i] = current_box;
	}

	current_box.invalidate( );

	uptr best_left_count;
	float sah = FLT_MAX;

	for ( ptr i = count - 1; i > 0; --i )
	{
		aabb box;
		box.set_min_max( nodes[i]->aabb_min, nodes[i]->aabb_max );

		current_box.extrude( box );

		float const current_sah = aabbs[i].surface_area( ) + current_box.surface_area( );
		if ( current_sah < sah )
		{
			sah = current_sah;
			best_left_count = i;
		}
	}

	sah_sum = sah;
	return best_left_count;
}

void bvh::serialize( lib::writer& w )
{
	if ( m_root == invalid )
	{
		w.write<u16>( 0 );
		return;
	}

	u16* const node_count_ptr = w.ptr( );
	w += sizeof(u16);

	struct queue_element
	{
		node_handle node_index;
		node_handle parent_index;
	};
	lib::linear_queue<queue_element> q;
	uptr const queue_size = 16384;
	q.create( stack_allocate( queue_size * sizeof(node_data*) ), queue_size );

	q.push( { m_root, invalid } );
	u16 nodes_count = 1;
	u16 current_node = 0;

	do
	{
		queue_element qe;
		q.pop( qe );

		node_data* const n = m_allocator[qe.node_index];

		w.write<float3>( n->aabb_min );

		if ( n->leaf_flag != 0 )
			w.write<u32>( n->obj );
		else
		{
			q.push( { n->left, current_node } );
			q.push( { n->right, current_node } );

			w.write<u16>( nodes_count++ );
			w.write<u16>( nodes_count++ );
		}

		w.write<float3>( n->aabb_max );
		w.write<u16>( qe.parent_index );
		w.write<u16>( n->leaf_flag );

		++current_node;
	}
	while ( !q.empty( ) );

	*node_count_ptr = nodes_count;
}

void bvh::deserialize( lib::reader& r, object_handle const* const handles )
{
	u16 const node_count = r.read<u16>( );

	uptr const size = node_count * sizeof(node_data);

	m_allocator.allocate_from_begin( size );

	pointer const data = r.read_data( size );
	memory::copy( m_allocator.data( ), data, size );

	for ( u16 i = 0; i < node_count; ++i )
	{
		node_data* const n = m_allocator[i];

		if ( n->leaf_flag != 0 )
			n->obj = handles[n->obj];
	}

	m_root = ( node_count != 0 ) ? 0 : invalid;
}

bvh::node_handle bvh::insert( object_handle const object, aabb const& object_aabb )
{
	node_data* const new_node = m_allocator.allocate( );
	new_node->aabb_min = object_aabb.get_min( );
	new_node->aabb_max = object_aabb.get_max( );
	new_node->obj = object;
	new_node->leaf_flag = 1;

	insert_impl( new_node );

	return (node_handle)m_allocator.index_of( new_node );
}

void bvh::insert_impl( node_data* const new_node )
{
	node_handle const new_node_handle = (node_handle)m_allocator.index_of( new_node );

	if ( m_root == invalid )
	{
		new_node->parent = invalid;
		m_root = new_node_handle;
		return;
	}

	node_handle handle = m_root;
	node_data* node = m_allocator[handle];

	node->aabb_min = min( node->aabb_min, new_node->aabb_min );
	node->aabb_max = max( node->aabb_max, new_node->aabb_max );

	while ( node->leaf_flag == 0 )
	{
		node_data* const left_node = m_allocator[node->left];
		aabb left_node_bbox;
		left_node_bbox.set_min_max(
			min( left_node->aabb_min, new_node->aabb_min ),
			max( left_node->aabb_max, new_node->aabb_max )
		);
		float const left_sah = left_node_bbox.surface_area( );

		node_data* const right_node = m_allocator[node->right];
		aabb right_node_bbox;
		right_node_bbox.set_min_max(
			min( right_node->aabb_min, new_node->aabb_min ),
			max( right_node->aabb_max, new_node->aabb_max )
		);
		float const right_sah = right_node_bbox.surface_area( );

		if ( left_sah < right_sah )
		{
			left_node->aabb_min = left_node_bbox.get_min( );
			left_node->aabb_max = left_node_bbox.get_max( );

			handle = node->left;
		}
		else
		{
			right_node->aabb_min = right_node_bbox.get_min( );
			right_node->aabb_max = right_node_bbox.get_max( );

			handle = node->right;
		}
		
		node = m_allocator[handle];
	}

	node_data* const parent_node = m_allocator.allocate( );
	node_handle const parent_node_handle = (node_handle)m_allocator.index_of( parent_node );
	parent_node->aabb_min = min( new_node->aabb_min, node->aabb_min );
	parent_node->aabb_max = max( new_node->aabb_max, node->aabb_max );
	parent_node->left = new_node_handle;
	parent_node->right = handle;
	parent_node->parent = node->parent;
	parent_node->leaf_flag = 0;

	if ( node->parent != invalid )
	{
		node_data* const grand_parent_node = m_allocator[node->parent];
		if ( grand_parent_node->left == handle )
			grand_parent_node->left = parent_node_handle;
		else
			grand_parent_node->right = parent_node_handle;
	}
	else
		m_root = parent_node_handle;

	new_node->parent = parent_node_handle;
	node->parent = parent_node_handle;
}

void bvh::remove( node_handle const handle )
{
	remove_impl( handle );

	m_allocator.deallocate( m_allocator[handle] );
}

void bvh::remove_impl( node_handle const handle )
{
	if ( handle == m_root )
	{
		m_root = invalid;
		return;
	}

	node_data* const node = m_allocator[handle];

	node_data* const parent = m_allocator[node->parent];
	node_handle const new_child_handle = ( parent->left == handle ) ? parent->right : parent->left;
	node_data* const new_child = m_allocator[new_child_handle];

	node_handle const grand_parent_handle = parent->parent;

	if ( grand_parent_handle == invalid )
	{
		m_root = new_child_handle;
		new_child->parent = invalid;
	}
	else
	{
		node_data* const grand_parent = m_allocator[grand_parent_handle];

		node_handle& parent_child_ptr = ( grand_parent->left == node->parent ) ? grand_parent->left : grand_parent->right;
		parent_child_ptr = new_child_handle;

		new_child->parent = grand_parent_handle;

		node_handle h = grand_parent_handle;

		do
		{
			node_data* const n = m_allocator[h];

			refit_node( *n );

			h = n->parent;
		}
		while ( h != invalid );
	}

	m_allocator.deallocate( parent );
}

void bvh::move( node_handle const handle, aabb const& node_aabb )
{
	node_data* const target = m_allocator[handle];

	// Update node AABB.
	target->aabb_min = node_aabb.get_min( );
	target->aabb_max = node_aabb.get_max( );

	// If it's root node, that's all.
	if ( target->parent == invalid )
		return;

	node_handle const parent_node_handle = target->parent;
	node_data* const parent_node = m_allocator[parent_node_handle];

	{
		bool const contains = ( parent_node->aabb_min < node_aabb.get_min( ) ) && ( parent_node->aabb_max > node_aabb.get_max( ) );

		// If parent node contains new AABB, or it's root node, then refit it and exit.
		if ( ( parent_node->parent == invalid ) || contains )
		{
			refit_node( *parent_node );
			return;
		}

		// Else, remove target and parent nodes from the old location.
		node_handle const new_child_handle = ( parent_node->left == handle ) ? parent_node->right : parent_node->left;
		node_data* const new_child = m_allocator[new_child_handle];

		new_child->parent = parent_node->parent;

		node_data* const grandparent = m_allocator[parent_node->parent];
		if ( grandparent->left == parent_node_handle )
			grandparent->left = new_child_handle;
		else
			grandparent->right = new_child_handle;
	}

	node_handle child = parent_node_handle;
	node_handle parent = parent_node->parent;
	bool contains = false;

	// Traverse tree to the top, search for node, that contains target.
	while ( parent != invalid )
	{
		node_data* const n = m_allocator[parent];

		contains = ( n->aabb_min < node_aabb.get_min( ) ) && ( n->aabb_max > node_aabb.get_max( ) );
		if ( contains )
			break;

		refit_node( *n );

		child = parent;
		parent = n->parent;
	}

	if ( !contains )
	{
		// If so, child node is root, refit it to contain target.
		node_data* const n = m_allocator[child];

		n->aabb_min = min( n->aabb_min, node_aabb.get_min( ) );
		n->aabb_max = max( n->aabb_max, node_aabb.get_max( ) );

		parent = child;
	}

	// Insert target node, starting from current.
	node_handle current = parent;
	node_data* node = m_allocator[current];

	while ( node->leaf_flag == 0 )
	{
		node_data* const left_node = m_allocator[node->left];
		aabb left_node_bbox;
		left_node_bbox.set_min_max(
			min( left_node->aabb_min, node_aabb.get_min( ) ),
			max( left_node->aabb_max, node_aabb.get_max( ) )
		);
		float const left_sah = left_node_bbox.surface_area( );

		node_data* const right_node = m_allocator[node->right];
		aabb right_node_bbox;
		right_node_bbox.set_min_max(
			min( right_node->aabb_min, node_aabb.get_min( ) ),
			max( right_node->aabb_max, node_aabb.get_max( ) )
		);
		float const right_sah = right_node_bbox.surface_area( );

		if ( left_sah < right_sah )
		{
			left_node->aabb_min = left_node_bbox.get_min( );
			left_node->aabb_max = left_node_bbox.get_max( );

			current = node->left;
		}
		else
		{
			right_node->aabb_min = right_node_bbox.get_min( );
			right_node->aabb_max = right_node_bbox.get_max( );

			current = node->right;
		}
		
		node = m_allocator[current];
	}

	// Found insert place, use parent node as common parent.

	// Update all links.
	node_data* const grandparent = m_allocator[node->parent];
	if ( grandparent->left == current )
		grandparent->left = parent_node_handle;
	else
		grandparent->right = parent_node_handle;

	parent_node->parent = node->parent;

	node->parent = parent_node_handle;

	if ( parent_node->left == handle )
		parent_node->right = current;
	else
		parent_node->left = current;

	// Refit parent node.
	refit_node( *parent_node );
}

void bvh::clear( )
{
	m_allocator.clear( );
	m_root = invalid;
}

math::aabb bvh::get_aabb( ) const
{
	ASSERT( m_root != invalid );
	node_data* const root = m_allocator[m_root];

	math::aabb result;
	result.set_min_max( root->aabb_min, root->aabb_max );
	return result;
}

void bvh::refit_node( node_data& node ) const
{
	node_data* const l = m_allocator[node.left];
	node_data* const r = m_allocator[node.right];

	node.aabb_min = min( l->aabb_min, r->aabb_min );
	node.aabb_max = max( l->aabb_max, r->aabb_max );
}

} // namespace math