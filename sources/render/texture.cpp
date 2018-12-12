#include "texture.h"

#include <lib/allocator.h>
#include <system/file.h>

#include "dx_include.h"
#include "api.h"
#include "resources.h"

namespace render {

u32 texture::add_ref( ) const
{
	return m_srv->AddRef( );
}

u32 texture::release( ) const
{
	return m_srv->Release( );
}

texture* texture::from_handle( texture_id const in_id )
{
	return g_resources.get_texture_pool( )[in_id];
}

texture_id texture::to_handle( texture* const in_texture )
{
	return (texture_id)g_resources.get_texture_pool( ).index_of( in_texture );
}

void texture::set_registry_pointer( pointer const in_pointer )
{
	m_registry_pointer = in_pointer;
}

pointer texture::get_registry_pointer( ) const
{
	return m_registry_pointer;
}

void create_resource(
	u32							in_resource_dimension,
	u32							in_width,
	u32							in_height,
	u32							in_depth,
	u32							in_mip_count,
	u32							in_array_size,
	DXGI_FORMAT					in_format,
	D3D11_USAGE					in_usage,
	u32							in_bind_flags,
	u32							in_cpu_access_flags,
	u32							in_misc_flags,
	bool						in_force_srgb,
	bool						in_is_cube_map,
	D3D11_SUBRESOURCE_DATA*		in_data,
	ID3D11ShaderResourceView**	out_srv )
{
	*out_srv = nullptr;

	if ( in_force_srgb )
		in_format = format_make_srgb( in_format );

	switch ( in_resource_dimension )
	{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		{
			D3D11_TEXTURE1D_DESC desc;
			desc.Width			= in_width;
			desc.MipLevels		= in_mip_count;
			desc.ArraySize		= in_array_size;
			desc.Format			= in_format;
			desc.Usage			= in_usage;
			desc.BindFlags		= in_bind_flags;
			desc.CPUAccessFlags	= in_cpu_access_flags;
			desc.MiscFlags		= in_misc_flags;

			ID3D11Texture1D* texture = nullptr;
			g_api.get_device( )->CreateTexture1D( &desc, in_data, &texture );

			ASSERT				( texture != nullptr );

			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			srv_desc.Format			= in_format;

			if ( in_array_size > 1 )
			{
				srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
				srv_desc.Texture1DArray.MipLevels	= in_mip_count ? desc.MipLevels : (u32)-1;
				srv_desc.Texture1DArray.ArraySize	= in_array_size;
			}
			else
			{
				srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE1D;
				srv_desc.Texture1D.MipLevels		= in_mip_count ? desc.MipLevels : (u32)-1;
			}

			g_api.get_device( )->CreateShaderResourceView( texture, &srv_desc, out_srv );
			
			ASSERT( *out_srv != nullptr );

			texture->Release( );
			break;
		}

		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Width				= in_width;
			desc.Height				= in_height;
			desc.MipLevels			= in_mip_count;
			desc.ArraySize			= in_array_size;
			desc.Format				= in_format;
			desc.SampleDesc.Count	= 1;
			desc.SampleDesc.Quality	= 0;
			desc.Usage				= in_usage;
			desc.BindFlags			= in_bind_flags;
			desc.CPUAccessFlags		= in_cpu_access_flags;
		
			desc.MiscFlags			= in_misc_flags;
			if ( in_is_cube_map )
				desc.MiscFlags		|= D3D11_RESOURCE_MISC_TEXTURECUBE;

			ID3D11Texture2D* texture = nullptr;
			g_api.get_device( )->CreateTexture2D( &desc, in_data, &texture );
			
			ASSERT( texture != nullptr );

			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			srv_desc.Format			= in_format;

			if ( in_is_cube_map )
			{
				if ( in_array_size > 6 )
				{
					srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					srv_desc.TextureCubeArray.MipLevels	= ( in_mip_count == 0 ) ? -1 : desc.MipLevels;
					srv_desc.TextureCubeArray.NumCubes	= in_array_size / 6;
				}
				else
				{
					srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURECUBE;
					srv_desc.TextureCube.MipLevels		= ( in_mip_count == 0 ) ? -1 : desc.MipLevels;
				}
			}
			else
			{
				if ( in_array_size > 1 )
				{
					srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					srv_desc.Texture2DArray.MipLevels	= ( in_mip_count == 0 ) ? -1 : desc.MipLevels;
					srv_desc.Texture2DArray.ArraySize	= in_array_size;
				}
				else
				{
					srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
					srv_desc.Texture2D.MipLevels		= ( in_mip_count == 0 ) ? -1 : desc.MipLevels;
				}
			}

			g_api.get_device( )->CreateShaderResourceView( texture, &srv_desc, out_srv );
			
			ASSERT( *out_srv != nullptr );

			texture->Release( );
			break;
		}

		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		{
			D3D11_TEXTURE3D_DESC desc;
			desc.Width			= in_width;
			desc.Height			= in_height;
			desc.Depth			= in_depth;
			desc.MipLevels		= in_mip_count;
			desc.Format			= in_format;
			desc.Usage			= in_usage;
			desc.BindFlags		= in_bind_flags;
			desc.CPUAccessFlags	= in_cpu_access_flags;
			desc.MiscFlags		= in_misc_flags;

			ID3D11Texture3D* texture = nullptr;
			g_api.get_device( )->CreateTexture3D( &desc, in_data, &texture );

			ASSERT( texture != nullptr );

			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			srv_desc.Format		= in_format;

			srv_desc.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE3D;
			srv_desc.Texture3D.MipLevels	= ( !in_mip_count ) ? -1 : desc.MipLevels;

			g_api.get_device( )->CreateShaderResourceView( texture, &srv_desc, out_srv );
			
			ASSERT( *out_srv != nullptr );

			texture->Release();
			break;
		}
	}
}

static const u32 c_dds_magic = 0x20534444; // "DDS "

struct dds_pixel_format
{
	u32 size;
	u32 flags;
	u32 fourcc;
	u32 rgb_bit_count;
	u32 r_bit_mask;
	u32 g_bit_mask;
	u32 b_bit_mask;
	u32 a_bit_mask;
};

static const u32 c_dds_fourcc				= 0x00000004;
static const u32 c_dds_rgb					= 0x00000040;
static const u32 c_dds_luminance			= 0x00020000;
static const u32 c_dds_alpha				= 0x00000002;
static const u32 c_dds_bumpdudv				= 0x00080000;

static const u32 c_dds_header_flags_volume	= 0x00800000;

static const u32 c_dds_height				= 0x00000002;
static const u32 c_dds_width				= 0x00000004;

static const u32 c_dds_cubemap_positive_x	= 0x00000600;
static const u32 c_dds_cubemap_negative_x	= 0x00000a00;
static const u32 c_dds_cubemap_positive_y	= 0x00001200;
static const u32 c_dds_cubemap_negative_y	= 0x00002200;
static const u32 c_dds_cubemap_positive_z	= 0x00004200;
static const u32 c_dds_cubemap_negative_z	= 0x00008200;

static const u32 c_dds_cubemap_all_faces	=	c_dds_cubemap_positive_x | c_dds_cubemap_negative_x |
												c_dds_cubemap_positive_y | c_dds_cubemap_negative_y |
												c_dds_cubemap_positive_z | c_dds_cubemap_negative_z;

static const u32 c_dds_cubemap				= 0x00000200;

enum dds_misc_flags2
{
	dds_misc_flags2_alpha_mode_mask = 0x7L,
};

struct dds_header
{
	u32					size;
	u32					flags;
	u32					height;
	u32					width;
	u32					pitch_or_linear_size;
	u32					depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	u32					mip_map_count;
	u32					reserved1[11];
	dds_pixel_format	ddspf;
	u32					caps;
	u32					caps2;
	u32					caps3;
	u32					caps4;
	u32					reserved2;
};

struct dds_header_dxt10
{
	DXGI_FORMAT	format;
	u32			resource_dimension;
	u32			misc_flag; // D3D11_RESOURCE_MISC_FLAG
	u32			array_size;
	u32			misc_flags2;
};

#define ISBITMASK( r,g,b,a ) ( ddpf.r_bit_mask == r && ddpf.g_bit_mask == g && ddpf.b_bit_mask == b && ddpf.a_bit_mask == a )

DXGI_FORMAT dds_to_dxgi_format( dds_pixel_format const& ddpf )
{
	if (ddpf.flags & c_dds_rgb )
	{
		switch ( ddpf.rgb_bit_count )
		{
		case 32:
			if ( ISBITMASK( 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 ) )
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			if ( ISBITMASK( 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 ) )
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			if ( ISBITMASK( 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 ) )
				return DXGI_FORMAT_B8G8R8X8_UNORM;
			if ( ISBITMASK( 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 ) )
				return DXGI_FORMAT_R10G10B10A2_UNORM;
			if ( ISBITMASK( 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 ) )
				return DXGI_FORMAT_R16G16_UNORM;
			if ( ISBITMASK( 0xffffffff, 0x00000000, 0x00000000, 0x00000000 ) )
				return DXGI_FORMAT_R32_FLOAT;
			break;

		case 24:
			break;

		case 16:
			if ( ISBITMASK( 0x7c00, 0x03e0, 0x001f, 0x8000 ) )
				return DXGI_FORMAT_B5G5R5A1_UNORM;
			if ( ISBITMASK( 0xf800, 0x07e0, 0x001f, 0x0000 ) )
				return DXGI_FORMAT_B5G6R5_UNORM;
			break;
		}
	}
	else if ( ddpf.flags & c_dds_luminance )
	{
		switch( ddpf.rgb_bit_count )
		{
		case 8:
			if ( ISBITMASK( 0x000000ff, 0x00000000, 0x00000000, 0x00000000 ) )
				return DXGI_FORMAT_R8_UNORM;
			if ( ISBITMASK( 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00 ) )
				return DXGI_FORMAT_R8G8_UNORM;
			break;
		
		case 16:
			if ( ISBITMASK( 0x0000ffff, 0x00000000, 0x00000000, 0x00000000 ) )
				return DXGI_FORMAT_R16_UNORM;
			if ( ISBITMASK( 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00 ) )
				return DXGI_FORMAT_R8G8_UNORM;
			break;
		}
	}
	else if ( ddpf.flags & c_dds_alpha )
	{
		if ( ddpf.rgb_bit_count == 8 )
			return DXGI_FORMAT_A8_UNORM;
	}
	else if ( ddpf.flags & c_dds_bumpdudv )
	{
		if ( ddpf.rgb_bit_count == 16 )
			if ( ISBITMASK( 0x00ff, 0xff00, 0x0000, 0x0000 ) )
				return DXGI_FORMAT_R8G8_SNORM;

		if ( ddpf.rgb_bit_count == 32 )
		{
			if ( ISBITMASK( 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 ) )
				return DXGI_FORMAT_R8G8B8A8_SNORM;
			if ( ISBITMASK( 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 ) )
				return DXGI_FORMAT_R16G16_SNORM;
		}
	}
	else if ( ddpf.flags & c_dds_fourcc )
	{
		if ( ddpf.fourcc == MAKEFOURCC('D', 'X', 'T', '1') )
			return DXGI_FORMAT_BC1_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('D', 'X', 'T', '3') )
			return DXGI_FORMAT_BC2_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('D', 'X', 'T', '5') )
			return DXGI_FORMAT_BC3_UNORM;

		if ( ddpf.fourcc == MAKEFOURCC('D', 'X', 'T', '2') )
			return DXGI_FORMAT_BC2_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('D', 'X', 'T', '4') )
			return DXGI_FORMAT_BC3_UNORM;

		if ( ddpf.fourcc == MAKEFOURCC('A', 'T', 'I', '1') )
			return DXGI_FORMAT_BC4_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('B', 'C', '4', 'U') )
			return DXGI_FORMAT_BC4_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('B', 'C', '4', 'S') )
			return DXGI_FORMAT_BC4_SNORM;

		if ( ddpf.fourcc == MAKEFOURCC('A', 'T', 'I', '2') )
			return DXGI_FORMAT_BC5_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('B', 'C', '5', 'U') )
			return DXGI_FORMAT_BC5_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('B', 'C', '5', 'S') )
			return DXGI_FORMAT_BC5_SNORM;

		if ( ddpf.fourcc == MAKEFOURCC('R', 'G', 'B', 'G') )
			return DXGI_FORMAT_R8G8_B8G8_UNORM;
		if ( ddpf.fourcc == MAKEFOURCC('G', 'R', 'G', 'B') )
			return DXGI_FORMAT_G8R8_G8B8_UNORM;

		switch ( ddpf.fourcc )
		{
		case 36:
			return DXGI_FORMAT_R16G16B16A16_UNORM;

		case 110:
			return DXGI_FORMAT_R16G16B16A16_SNORM;

		case 111:
			return DXGI_FORMAT_R16_FLOAT;

		case 112:
			return DXGI_FORMAT_R16G16_FLOAT;

		case 113:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case 114:
			return DXGI_FORMAT_R32_FLOAT;

		case 115:
			return DXGI_FORMAT_R32G32_FLOAT;

		case 116:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}
void get_surface_info( uptr			width,
					   uptr			height,
					   DXGI_FORMAT	fmt,
					   uptr&		out_num_bytes,
					   uptr&		out_row_bytes )
{
	if ( format_is_dxt( fmt ) )
	{
		size_t num_blocks_wide	= ( width > 0 ) ? ( width + 3 ) / 4 : 0;
		uptr num_blocks_high	= (height > 0) ? ( height + 3 ) / 4 : 0;

		out_row_bytes			= num_blocks_wide * format_get_bits_per_pixel( fmt );
		out_num_bytes			= out_row_bytes * num_blocks_high;
	}
	else if ( ( fmt == DXGI_FORMAT_R8G8_B8G8_UNORM ) ||
			  ( fmt == DXGI_FORMAT_G8R8_G8B8_UNORM ) )
	{
		out_row_bytes			= ( ( width + 1 ) >> 1 ) * 4;
		out_num_bytes			= out_row_bytes * height;
	}
	else
	{
		size_t bpp				= format_get_bits_per_pixel( fmt );
		out_row_bytes			= (width * bpp + 7) / 8;
		out_num_bytes			= out_row_bytes * height;
	}
}

void fill_init_data( uptr						width,
					 uptr						height,
					 uptr						depth,
					 uptr						mip_count,
					 uptr						array_size,
					 DXGI_FORMAT				format,
					 binary_config&				config,
					 D3D11_SUBRESOURCE_DATA*	init_data )
{
	ASSERT( init_data );

	uptr num_bytes = 0;
	uptr row_bytes = 0;

	uptr index = 0;
	for ( uptr j = 0; j < array_size; ++j )
	{
		uptr w = width;
		uptr h = height;
		uptr d = depth;

		for ( size_t i = 0; i < mip_count; ++i )
		{
			get_surface_info( w, h, format, num_bytes, row_bytes );

			ASSERT( index < mip_count * array_size );

			init_data[index].pSysMem			= config.read_data( num_bytes * d );
			init_data[index].SysMemPitch		= (UINT)row_bytes;
			init_data[index].SysMemSlicePitch	= (UINT)num_bytes;
			++index;

			w = ( w != 1 ) ? w >> 1 : 1;
			h = ( h != 1 ) ? h >> 1 : 1;
			d = ( d != 1 ) ? d >> 1 : 1;
		}
	}
}

void texture::create( binary_config& in_config )
{
	ASSERT( in_config.is_valid( ) );

	pvoid const memory	= in_config.get_pointer( );
	u32 magic_number	= in_config.read<u32>( );
	ASSERT				( magic_number == c_dds_magic );

	dds_header const& header = in_config.read<dds_header>( );
	ASSERT( header.size == sizeof(dds_header) );
	ASSERT( header.ddspf.size == sizeof(dds_pixel_format) );
	
	u32 width			= header.width;
	u32 height			= header.height;
	u32 depth			= header.depth;

	u32 res_dim			= D3D11_RESOURCE_DIMENSION_UNKNOWN;
	u32 array_size		= 1;
	DXGI_FORMAT format	= DXGI_FORMAT_UNKNOWN;
	bool is_cube_map	= false;
	
	uptr mip_count		= header.mip_map_count;
	if ( mip_count == 0 )
		mip_count = 1;

	if ( ( header.ddspf.flags & c_dds_fourcc ) &&
		( MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourcc ) )
	{
		dds_header_dxt10 const& dxt_header = in_config.read<dds_header_dxt10>( );

		array_size = dxt_header.array_size;
		ASSERT( array_size );

		format = dxt_header.format;
		ASSERT( format_get_bits_per_pixel( dxt_header.format ) != 0 );
		
		res_dim = dxt_header.resource_dimension;
		switch ( res_dim )
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			if ( header.flags & c_dds_height )
				ASSERT( height == 1 );

			height = depth = 1;
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			if ( dxt_header.misc_flag & D3D11_RESOURCE_MISC_TEXTURECUBE )
			{
				array_size	*= 6;
				is_cube_map	= true;
			}
			depth = 1;
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			ASSERT( header.flags & c_dds_header_flags_volume );
			ASSERT( array_size == 1 );
			break;

		default:
			UNREACHABLE_CODE
		}
	}
	else
	{
		format = dds_to_dxgi_format( header.ddspf );
		ASSERT( format != DXGI_FORMAT_UNKNOWN );

		if ( header.flags & c_dds_header_flags_volume )
			res_dim = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
		else
		{
			if ( header.caps2 & c_dds_cubemap )
			{
				ASSERT( ( header.caps2 & c_dds_cubemap_all_faces ) == c_dds_cubemap_all_faces );

				array_size	= 6;
				is_cube_map	= true;
			}

			depth			= 1;
			res_dim			= D3D11_RESOURCE_DIMENSION_TEXTURE2D;
		}

		ASSERT( format_get_bits_per_pixel( format ) != 0 );
	}

	ASSERT( mip_count <= D3D11_REQ_MIP_LEVELS );

	switch ( res_dim )
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		ASSERT( array_size <= D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION );
		ASSERT( width <= D3D11_REQ_TEXTURE1D_U_DIMENSION );
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		ASSERT( array_size <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION );

		if ( is_cube_map )
		{
			ASSERT( width <= D3D11_REQ_TEXTURECUBE_DIMENSION );
			ASSERT( height <= D3D11_REQ_TEXTURECUBE_DIMENSION );
		}
		else
		{
			ASSERT( width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION );
			ASSERT( height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION );
		}
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		ASSERT( array_size <= 1 );
		ASSERT( width <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );
		ASSERT( height <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );
		ASSERT( depth <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );
		break;

	default:
		UNREACHABLE_CODE
	}

	D3D11_SUBRESOURCE_DATA* init_data = stack_allocate( mip_count * array_size * sizeof(D3D11_SUBRESOURCE_DATA) );

	fill_init_data( width, height, depth, mip_count, array_size, format, in_config, init_data );

	create_resource( res_dim, width, height, depth, (u32)mip_count, array_size,
					 format, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE,
					 0, 0, false, is_cube_map, init_data, &m_srv );
}

} // namespace render