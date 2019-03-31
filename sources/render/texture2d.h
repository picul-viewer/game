#ifndef GUARD_RENDER_TEXTURE2D_H_INCLUDED
#define GUARD_RENDER_TEXTURE2D_H_INCLUDED

#include <types.h>
#include "dx_include.h"

namespace render {

class texture2d
{
public:
	struct cook
	{
		void set_render_target( u32			in_width,
								u32			in_height,
								DXGI_FORMAT	in_format,
								bool		in_is_srv = false,
								bool		in_is_dynamic = false,
								u32			in_mips = 1,
								u32			in_array_size = 1 );

		void set_texture2d( u32			in_width,
							u32			in_height,
							u32			in_mips,
							u32			in_array_size,
							DXGI_FORMAT	in_format,
							u32			in_sample_count,
							u32			in_sample_quality,
							D3D11_USAGE	in_usage,
							u32			in_bind_flags,
							u32			in_cpu_access_flags,
							u32			in_misc_flags );

		D3D11_TEXTURE2D_DESC desc;
	};
	
	texture2d( );

	void create( cook const& in_cook );
	void destroy( );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	inline void set( ID3D11Texture2D* in_texture ) { m_texture = in_texture; }
	inline ID3D11Texture2D* const& get( ) const { return m_texture; }

protected:
	ID3D11Texture2D*	m_texture;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_TEXTURE2D_H_INCLUDED
