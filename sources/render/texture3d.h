#ifndef GUARD_RENDER_TEXTURE3D_H_INCLUDED
#define GUARD_RENDER_TEXTURE3D_H_INCLUDED

#include <types.h>
#include "dx_include.h"

namespace render {

class texture3d
{
public:
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
	
	texture3d( );

	void create( cook const& in_cook );
	void destroy( );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	inline void set( ID3D11Texture3D* in_texture ) { m_texture = in_texture; }
	inline ID3D11Texture3D* const& get( ) const { return m_texture; }

protected:
	ID3D11Texture3D*	m_texture;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_TEXTURE3D_H_INCLUDED
