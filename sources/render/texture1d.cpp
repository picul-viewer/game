#include "texture1d.h"
#include "render_api.h"

namespace render {

void texture1d::cook::set_texture1d( u32			in_width,
									 u32			in_mips,
									 u32			in_array_size,
									 DXGI_FORMAT	in_format,
									 D3D11_USAGE	in_usage,
									 u32			in_bind_flags,
									 u32			in_cpu_access_flags,
									 u32			in_misc_flags )
{
	desc.Width				= in_width;
	desc.MipLevels			= in_mips;
	desc.ArraySize			= in_array_size;
	desc.Format				= in_format;
	desc.Usage				= in_usage;
	desc.BindFlags			= in_bind_flags;
	desc.CPUAccessFlags		= in_cpu_access_flags;
	desc.MiscFlags			= in_misc_flags;
}

texture1d::texture1d( ) :
	m_texture( nullptr )
{ }

void texture1d::create( cook const& in_cook )
{
	g_api.get_device( )->CreateTexture1D( &in_cook.desc, nullptr, &m_texture );
}

void texture1d::destroy( )
{
	dx_release( m_texture );
}

u32 texture1d::add_ref( ) const
{
	return m_texture->AddRef( );
}

u32 texture1d::release( ) const
{
	return m_texture->Release( );
}

} // namespace render