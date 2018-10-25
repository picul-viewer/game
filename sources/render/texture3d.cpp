#include "texture3d.h"
#include "api.h"

namespace render {

void texture3d::cook::set_texture3d( u32			in_width,
									 u32			in_height,
									 u32			in_depth,
									 u32			in_mips,
									 DXGI_FORMAT	in_format,
									 D3D11_USAGE	in_usage,
									 u32			in_bind_flags,
									 u32			in_cpu_access_flags,
									 u32			in_misc_flags )
{
	desc.Width				= in_width;
	desc.Height				= in_height;
	desc.Depth				= in_depth;
	desc.MipLevels			= in_mips;
	desc.Format				= in_format;
	desc.Usage				= in_usage;
	desc.BindFlags			= in_bind_flags;
	desc.CPUAccessFlags		= in_cpu_access_flags;
	desc.MiscFlags			= in_misc_flags;
}

void texture3d::create( cook const& in_cook )
{
	g_api.get_device( )->CreateTexture3D( &in_cook.desc, nullptr, &m_texture );
}

texture3d::texture3d( ) :
	m_texture( nullptr )
{ }

void texture3d::destroy( )
{
	dx_release( m_texture );
}

u32 texture3d::add_ref( ) const
{
	return m_texture->AddRef( );
}

u32 texture3d::release( ) const
{
	return m_texture->Release( );
}

} // namespace render