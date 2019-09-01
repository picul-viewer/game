#ifndef GUARD_RENDER_OBJECT_H_INCLUDED
#define GUARD_RENDER_OBJECT_H_INCLUDED

#include <types.h>
#include <math/matrix.h>
#include <lib/list.h>
#include <lib/reader.h>
#include <resource_system/default_resource.h>
#include <utils/engine_threads.h>

#include "render_object.h"

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
	inline render_object* data( ) const { return m_objects; }

	void update( math::float4x3 const& in_transform );

private:
	render_object* m_objects;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_OBJECT_H_INCLUDED