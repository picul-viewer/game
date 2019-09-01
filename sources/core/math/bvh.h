#ifndef GUARD_CORE_BVH_H_INCLUDED
#define GUARD_CORE_BVH_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <lib/pool_allocator.h>
#include <lib/reader.h>
#include <lib/writer.h>
#include "aabb.h"
#include "frustum.h"

namespace math {

class bvh
{
public:
	typedef u16 node_handle;
	typedef u32 object_handle;

	enum { node_data_size = 32 };

public:
	void create( pointer const memory, uptr const memory_size );
	void destroy( );
	
	void build( aabb const* const aabbs, object_handle const* const handles, uptr const count );
	void serialize( lib::writer& w );
	void deserialize( lib::reader& r, object_handle const* const handles );

	node_handle insert( object_handle const object, aabb const& object_aabb );
	void remove( node_handle const handle );
	void move( node_handle const handle, aabb const& node_aabb );

	void clear( );

	template<typename Callback>
	void for_each( Callback const& callback ) const;

	template<typename Callback>
	void frustum_test( frustum const& f, Callback const& callback ) const;

private:
	mem_align(16)
	struct node_data
	{
		float3 aabb_min;
		union
		{
			struct
			{
				node_handle left;
				node_handle right;
			};
			object_handle obj;
		};
		float3 aabb_max;
		node_handle parent;
		u16 leaf_flag;
	};

	static const node_handle invalid = -1;

private:
	void insert_impl( node_data* const node );
	void remove_impl( node_handle const handle );

	node_handle build_impl(
		node_data** const nodes,
		node_data** const nodes_memory_0,
		node_data** const nodes_memory_1,
		aabb* const aabb_memory,
		uptr const count,
		u32 const sorting_component
	);

	uptr divide_by_sah(
		node_data** const nodes,
		aabb* const aabbs,
		uptr const count,
		float& sah_sum
	);

	template<typename Callback>
	void for_each_impl( Callback const& callback, node_data* const node ) const;

	template<typename Callback>
	void frustum_test_impl( frustum const& f, Callback const& callback, node_data* const node ) const;

	void refit_node( node_data& node ) const;

private:
	lib::pool_allocator<sizeof(node_data)> m_allocator;
	node_handle m_root;
};

} // namespace math

#include "bvh_inline.h"

#endif // #ifndef GUARD_CORE_BVH_H_INCLUDED