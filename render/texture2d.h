#ifndef __render_texture2d_h_included_
#define __render_texture2d_h_included_

#include <types.h>
#include "dx_include.h"

#include "resource_views.h"

namespace render {

class texture2d
{
public:
	texture2d( );

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

	void create( cook const& in_cook );
	void set( ID3D11Texture2D* in_texture );
	void destroy( );
	
	inline ID3D11Texture2D* get_resource( ) const { return m_texture; }
protected:
	ID3D11Texture2D*	m_texture;
};

} // namespace render

#endif // #ifndef __render_texture2d_h_included_
