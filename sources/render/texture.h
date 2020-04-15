#ifndef GUARD_RENDER_TEXTURE_H_INCLUDED
#define GUARD_RENDER_TEXTURE_H_INCLUDED

#include <types.h>

#include <resource_system/shared_resource.h>
#include <utils/engine_threads.h>

#include "dx_resource.h"
#include "handles.h"

namespace render {

class texture : public shared_resource<texture, texture_handle, 4096>
{
public:
	friend class texture_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_main
	};

	static void destroy_resource( texture* const in_resource );

public:
	inline u32 get( ) const { return m_texture_id; }

private:
	dx_resource m_texture;
	u32 m_texture_id;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_TEXTURE_H_INCLUDED