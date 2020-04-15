#include "dds_loader.h"

#include <macros.h>

namespace render {
namespace dds_loader {

enum : u32 {
	c_dds_magic					= 0x20534444, // "DDS "

	c_dds_fourcc				= 0x00000004,
	c_dds_rgb					= 0x00000040,
	c_dds_luminance				= 0x00020000,
	c_dds_alpha					= 0x00000002,
	c_dds_bumpdudv				= 0x00080000,
	
	c_dds_header_flags_volume	= 0x00800000,
	
	c_dds_height				= 0x00000002,
	c_dds_width					= 0x00000004,
	
	c_dds_cubemap_positive_x	= 0x00000600,
	c_dds_cubemap_negative_x	= 0x00000a00,
	c_dds_cubemap_positive_y	= 0x00001200,
	c_dds_cubemap_negative_y	= 0x00002200,
	c_dds_cubemap_positive_z	= 0x00004200,
	c_dds_cubemap_negative_z	= 0x00008200,
	
	c_dds_cubemap_all_faces		=	c_dds_cubemap_positive_x | c_dds_cubemap_negative_x |
									c_dds_cubemap_positive_y | c_dds_cubemap_negative_y |
									c_dds_cubemap_positive_z | c_dds_cubemap_negative_z,
	
	c_dds_cubemap				= 0x00000200

};

enum dds_misc_flags2
{
	dds_misc_flags2_alpha_mode_mask = 0x7L,
};

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


void generate_upload_tasks(
	u32 const width,
	u32 const height,
	u32 const depth,
	u32 const mip_count,
	u32 const array_size,
	DXGI_FORMAT const format,
	lib::reader& reader,
	dx_resource const resource,
	lib::buffer_array<gpu_upload_task>& upload_tasks
)
{
	bool const is_dxt = format_is_dxt( format );

	u32 num_bytes = 0;
	u32 row_bytes = 0;

	u32 index = 0;
	for ( u32 j = 0; j < array_size; ++j )
	{
		u32 w = width;
		u32 h = height;
		u32 d = depth;

		for ( u32 i = 0; i < mip_count; ++i )
		{
			if ( is_dxt )
			{
				u32 const num_blocks_wide = ( w > 0 ) ? ( w + 3 ) / 4 : 0;
				u32 const num_blocks_high = ( h > 0 ) ? ( h + 3 ) / 4 : 0;

				row_bytes = num_blocks_wide * format_get_bits_per_pixel( format );
				num_bytes = row_bytes * num_blocks_high;
			}
			else if ( ( format == DXGI_FORMAT_R8G8_B8G8_UNORM ) || ( format == DXGI_FORMAT_G8R8_G8B8_UNORM ) )
			{
				row_bytes = ( ( w + 1 ) >> 1 ) * 4;
				num_bytes = row_bytes * h;
			}
			else
			{
				u32 const bpp = format_get_bits_per_pixel( format );
				row_bytes = ( w * bpp + 7 ) / 8;
				num_bytes = row_bytes * h;
			}

			ASSERT( !is_dxt || ( ( w >= 4 ) && ( h >= 4 ) ) );

			gpu_upload_task& task = upload_tasks.emplace_back( );

			D3D12_SUBRESOURCE_FOOTPRINT footprint;
			footprint.Format = format;
			footprint.Width = w;
			footprint.Height = h;
			footprint.Depth = d;
			footprint.RowPitch = math::align_up( row_bytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT );

			task.set_texture_upload( resource, index, footprint );

			uptr const data_size = num_bytes * d;
			pvoid const data = reader.read_data( num_bytes * d );

			task.set_source_data( data, data_size, ( footprint.RowPitch == row_bytes ) ? 0 : row_bytes );

			++index;

			w = ( w != 1 ) ? w >> 1 : 1;
			h = ( h != 1 ) ? h >> 1 : 1;
			d = ( d != 1 ) ? d >> 1 : 1;
		}
	}
}

void load(
	lib::reader& in_reader,
	dx_resource& in_resource,
	D3D12_SHADER_RESOURCE_VIEW_DESC& in_srv_desc,
	lib::buffer_array<gpu_upload_task>& in_upload_tasks
)
{
	ASSERT				( in_reader.is_valid( ) );

	u32 magic_number	= in_reader.read<u32>( );
	ASSERT_CMP			( magic_number, ==, c_dds_magic );

	dds_header const& header = in_reader.read<dds_header>( );
	ASSERT_CMP			( header.size, ==, sizeof(dds_header) );
	ASSERT_CMP			( header.ddspf.size, ==, sizeof(dds_pixel_format) );
	
	u32 width			= header.width;
	u32 height			= header.height;
	u32 depth			= header.depth;

	u32 res_dim			= D3D12_RESOURCE_DIMENSION_UNKNOWN;
	u32 array_size		= 1;
	DXGI_FORMAT format	= DXGI_FORMAT_UNKNOWN;
	bool is_cube_map	= false;
	
	u32 mip_count		= header.mip_map_count;
	if ( mip_count == 0 )
		mip_count = 1;

	if ( ( header.ddspf.flags & c_dds_fourcc ) &&
		( MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourcc ) )
	{
		dds_header_dxt10 const& dxt_header = in_reader.read<dds_header_dxt10>( );

		array_size = dxt_header.array_size;
		ASSERT( array_size );

		format = dxt_header.format;
		ASSERT( format_get_bits_per_pixel( dxt_header.format ) != 0 );
		
		res_dim = dxt_header.resource_dimension;
		switch ( res_dim )
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if ( header.flags & c_dds_height )
				ASSERT_CMP( height, ==, 1 );

			height = depth = 1;
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			depth = 1;
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			ASSERT( header.flags & c_dds_header_flags_volume );
			ASSERT_CMP( array_size, ==, 1 );
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
			res_dim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		else
		{
			if ( header.caps2 & c_dds_cubemap )
			{
				ASSERT( ( header.caps2 & c_dds_cubemap_all_faces ) == c_dds_cubemap_all_faces );

				array_size	= 6;
				is_cube_map	= true;
			}

			depth			= 1;
			res_dim			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		}

		ASSERT( format_get_bits_per_pixel( format ) != 0 );
	}

	ASSERT_CMP( mip_count, <=, D3D12_REQ_MIP_LEVELS );


	in_srv_desc.Format = format;
	in_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch ( res_dim )
	{
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		{
			ASSERT_CMP( array_size, <=, D3D12_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION );
			ASSERT_CMP( width, <=, D3D12_REQ_TEXTURE1D_U_DIMENSION );

			dx_resource::cook cook;
			cook.create_texture1d(
				format, width, array_size, mip_count,
				true, false, false, false, false
			);
			cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
			cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );
			in_resource.create( cook );

			if ( array_size == 1 )
			{
				in_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
				in_srv_desc.Texture1D.MostDetailedMip = 0;
				in_srv_desc.Texture1D.MipLevels = -1;
				in_srv_desc.Texture1D.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				in_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
				in_srv_desc.Texture1DArray.ArraySize = array_size;
				in_srv_desc.Texture1DArray.FirstArraySlice = 0;
				in_srv_desc.Texture1DArray.MostDetailedMip = 0;
				in_srv_desc.Texture1DArray.MipLevels = -1;
				in_srv_desc.Texture1DArray.ResourceMinLODClamp = 0.0f;
			}

			break;
		}

		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		{
			ASSERT_CMP( array_size, <=, D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION );

			if ( is_cube_map )
			{
				ASSERT_CMP( width, <=, D3D12_REQ_TEXTURECUBE_DIMENSION );
				ASSERT_CMP( height, <=, D3D12_REQ_TEXTURECUBE_DIMENSION );
			}
			else
			{
				ASSERT_CMP( width, <=, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION );
				ASSERT_CMP( height, <=, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION );
			}

			// Avoid too small images with DXT formats.
			bool const is_dxt = format_is_dxt( format );
			if ( is_dxt )
			{
				BOOLEAN result;
				u32 log_width, log_height;
				result = _BitScanForward( (DWORD*)&log_width, width / 4 );
				ASSERT( result );
				result = _BitScanForward( (DWORD*)&log_height, height / 4 );
				ASSERT( result );

				mip_count = math::min( mip_count, math::min( log_width, log_height ) + 1 );
			}

			dx_resource::cook cook;
			cook.create_texture2d(
				format, width, height, array_size, mip_count,
				true, false, false, false, false
			);
			cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
			cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );
			in_resource.create( cook );

			if ( is_cube_map )
			{
				in_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				in_srv_desc.TextureCube.MostDetailedMip = 0;
				in_srv_desc.TextureCube.MipLevels = -1;
				in_srv_desc.TextureCube.ResourceMinLODClamp = 0.0f;
			}
			else if ( array_size == 1 )
			{
				in_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				in_srv_desc.Texture2D.MostDetailedMip = 0;
				in_srv_desc.Texture2D.MipLevels = -1;
				in_srv_desc.Texture2D.PlaneSlice = 0;
				in_srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				in_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				in_srv_desc.Texture2DArray.ArraySize = array_size;
				in_srv_desc.Texture2DArray.FirstArraySlice = 0;
				in_srv_desc.Texture2DArray.MostDetailedMip = 0;
				in_srv_desc.Texture2DArray.MipLevels = -1;
				in_srv_desc.Texture2DArray.PlaneSlice = 0;
				in_srv_desc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			}

			break;
		}

		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		{
			ASSERT_CMP( array_size, <=, 1 );
			ASSERT_CMP( width, <=, D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );
			ASSERT_CMP( height, <=, D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );
			ASSERT_CMP( depth, <=, D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );

			// Avoid too small images with DXT formats.
			bool const is_dxt = format_is_dxt( format );
			if ( is_dxt )
			{
				BOOLEAN result;
				u32 log_width, log_height;
				result = _BitScanForward( (DWORD*)&log_width, width / 4 );
				ASSERT( result );
				result = _BitScanForward( (DWORD*)&log_height, height / 4 );
				ASSERT( result );

				mip_count = math::min( mip_count, math::min( log_width, log_height ) + 1 );
			}

			dx_resource::cook cook;
			cook.create_texture3d(
				format, width, height, depth, mip_count,
				true, false, false, false, false
			);
			cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
			cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );
			in_resource.create( cook );

			in_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			in_srv_desc.Texture3D.MostDetailedMip = 0;
			in_srv_desc.Texture3D.MipLevels = -1;
			in_srv_desc.Texture3D.ResourceMinLODClamp = 0.0f;

			break;
		}

		default:
			UNREACHABLE_CODE
	}


	generate_upload_tasks(
		width, height, depth, mip_count, array_size,
		format, in_reader, in_resource, in_upload_tasks
	);
}

} // namespace dds_loader
} // namespace render