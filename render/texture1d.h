#ifndef __render_texture1d_h_included_
#define __render_texture1d_h_included_

#include <types.h>
#include "dx_include.h"

namespace render {

class texture1d
{
public:
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
	
	texture1d( );

	void create( cook const& in_cook );
	void destroy( );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	inline void set( ID3D11Texture1D* in_texture ) { m_texture = in_texture; }
	inline ID3D11Texture1D* const& get( ) const { return m_texture; }

protected:
	ID3D11Texture1D*	m_texture;
};

} // namespace render

#endif // #ifndef __render_texture1d_h_included_
