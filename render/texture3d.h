#ifndef __render_texture3d_h_included_
#define __render_texture3d_h_included_

#include <types.h>
#include "dx_include.h"

#include "resource_views.h"

namespace render {

class texture3d
{
public:
	texture3d( );

	struct cook
	{
		void set_texture3d( u32			in_width,
							u32			in_height,
							u32			in_depth,
							u32			in_mips,
							DXGI_FORMAT	in_format,
							D3D11_USAGE	in_usage,
							u32			in_bind_flags,
							u32			in_cpu_access_flags,
							u32			in_misc_flags );

		D3D11_TEXTURE3D_DESC desc;
	};

	void create( cook const& in_cook );
	void set( ID3D11Texture3D* in_texture );
	void destroy( );
	
	inline ID3D11Texture3D* get_resource( ) const { return m_texture; }
protected:
	ID3D11Texture3D*	m_texture;
};

} // namespace render

#endif // #ifndef __render_texture3d_h_included_
