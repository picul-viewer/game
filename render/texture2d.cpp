#include "texture2d.h"

#include "render_api.h"
#include <fs/fs_core.h>

namespace render {

texture2d::texture2d( ) :
	m_texture( nullptr )
{ }

void texture2d::cook::set_render_target( u32			in_width,
										 u32			in_height,
										 DXGI_FORMAT	in_format,
										 bool			in_is_srv,
										 bool			in_is_dynamic,
										 u32			in_mips,
										 u32			in_array_size )
{
	desc.Width				= in_width;
	desc.Height				= in_height;
	desc.MipLevels			= in_mips;
	desc.ArraySize			= in_array_size;
	desc.Format				= in_format;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality	= 0;
	desc.Usage				= in_is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags			= in_is_srv ? D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags		= in_is_dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags			= ( in_mips != 1 ) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
}

void texture2d::cook::set_texture2d( u32			in_width,
									 u32			in_height,
									 u32			in_mips,
									 u32			in_array_size,
									 DXGI_FORMAT	in_format,
									 u32			in_sample_count,
									 u32			in_sample_quality,
									 D3D11_USAGE	in_usage,
									 u32			in_bind_flags,
									 u32			in_cpu_access_flags,
									 u32			in_misc_flags )
{
	desc.Width				= in_width;
	desc.Height				= in_height;
	desc.MipLevels			= in_mips;
	desc.ArraySize			= in_array_size;
	desc.Format				= in_format;
	desc.SampleDesc.Count	= in_sample_count;
	desc.SampleDesc.Quality	= in_sample_quality;
	desc.Usage				= in_usage;
	desc.BindFlags			= in_bind_flags;
	desc.CPUAccessFlags		= in_cpu_access_flags;
	desc.MiscFlags			= in_misc_flags;
}

void texture2d::create( cook const& in_cook )
{
	api::get_device( )->CreateTexture2D( &in_cook.desc, nullptr, &m_texture );
}

void texture2d::set( ID3D11Texture2D* in_texture )
{
	m_texture = in_texture;
}

void texture2d::destroy( )
{
	dx_release( m_texture );
}

} // namespace render