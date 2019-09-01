#ifndef GUARD_RENDER_TEXTURE_H_INCLUDED
#define GUARD_RENDER_TEXTURE_H_INCLUDED

#include <types.h>

#include <resource_system/shared_resource.h>
#include "resource_views.h"
#include "texture_cook.h"
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
	inline ID3D11ShaderResourceView* const& get( ) const { return m_view.get( ); }

	inline void bind_vs( u32 const in_slot, u32 const in_count = 1 ) const { m_view.bind_vs( in_slot, in_count ); }
	inline void bind_ps( u32 const in_slot, u32 const in_count = 1 ) const { m_view.bind_ps( in_slot, in_count ); }
	inline void bind_gs( u32 const in_slot, u32 const in_count = 1 ) const { m_view.bind_gs( in_slot, in_count ); }
	inline void bind_hs( u32 const in_slot, u32 const in_count = 1 ) const { m_view.bind_hs( in_slot, in_count ); }
	inline void bind_ds( u32 const in_slot, u32 const in_count = 1 ) const { m_view.bind_ds( in_slot, in_count ); }
	inline void bind_cs( u32 const in_slot, u32 const in_count = 1 ) const { m_view.bind_cs( in_slot, in_count ); }

private:
	shader_resource_view m_view;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_TEXTURE_H_INCLUDED