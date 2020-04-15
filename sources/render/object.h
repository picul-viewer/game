#ifndef GUARD_RENDER_OBJECT_H_INCLUDED
#define GUARD_RENDER_OBJECT_H_INCLUDED

#include <types.h>
#include <math/matrix.h>
#include <lib/list.h>
#include <lib/reader.h>
#include <resource_system/default_resource.h>
#include <utils/engine_threads.h>

namespace render {

class object : public default_resource<object>
{
public:
	friend class object_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_helper_thread_0
	};

	static void destroy_resource( object* const in_resource );

public:
	template<typename F>
	void for_each_mesh_object( F const& in_functor ) const;

	void update( math::float4x3 const& in_transform );

	static uptr calculate_allocation_size( u32 const in_mesh_count );

private:
	u32 m_mesh_count;
	u32 m_padding[3];

#pragma warning (push)
#pragma warning (disable:4200)
	u8 m_data[0];
#pragma warning (pop)

};

} // namespace render

#include "object_inline.h"

#endif // #ifndef GUARD_RENDER_OBJECT_H_INCLUDED