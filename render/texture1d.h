#ifndef __render_texture1d_h_included_
#define __render_texture1d_h_included_

#include <types.h>
#include "dx_include.h"

#include "resource_views.h"

namespace render {

class texture1d
{
public:
	texture1d( );

	struct cook
	{
		void set_texture1d( u32			in_width,
							u32			in_mips,
							u32			in_array_size,
							DXGI_FORMAT	in_format,
							D3D11_USAGE	in_usage,
							u32			in_bind_flags,
							u32			in_cpu_access_flags,
							u32			in_misc_flags );

		D3D11_TEXTURE1D_DESC desc;
	};

	void create( cook const& in_cook );
	void set( ID3D11Texture1D* in_texture );
	void destroy( );

	inline ID3D11Texture1D* get( ) const { return m_texture; }
protected:
	ID3D11Texture1D*	m_texture;
};

} // namespace render

#endif // #ifndef __render_texture1d_h_included_
