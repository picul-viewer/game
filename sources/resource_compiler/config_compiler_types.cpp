#include <types.h>
#include <lib/allocator.h>
#include <lib/text_reader.h>
#include <lib/writer.h>
#include <math/bvh.h>
#include <resources/resources_path.h>
#include <system/file.h>
#include <utils/config_reader.h>
#include "config_compiler_common.h"

namespace resource_compiler {

static bool save( pvoid const data, uptr const size, pcstr const output_path )
{
	sys::file f;
	f.create( output_path, sys::file::open_write );
	if ( !f.is_valid( ) )
		return false;

	f.write( data, size );
	f.destroy( );

	return true;
}


bool compile_config_font( pstr const data, uptr const size, pcstr const file_name, pcstr const output_path )
{
	lib::text_reader cfg( data, size );

	u32 image_width;
	u32 image_height;
	u32 cell_width;
	u32 cell_height;
	u32 start_char;
	u32 font_height;

	u8 chars_width[256];

	cfg.read_mask( TEXT_READER_MASK( "Image Width,%d\n" ), &image_width );
	cfg.read_mask( TEXT_READER_MASK( "Image Height,%d\n" ), &image_height );
	cfg.read_mask( TEXT_READER_MASK( "Cell Width,%d\n" ), &cell_width );
	cfg.read_mask( TEXT_READER_MASK( "Cell Height,%d\n" ), &cell_height );
	cfg.read_mask( TEXT_READER_MASK( "Start Char,%d\n" ), &start_char );
	cfg.skip_line( );
	cfg.read_mask( TEXT_READER_MASK( "Font Height,%d\n" ), &font_height );
	cfg.skip_line( );

	for ( u32 i = 0; i < start_char; ++i )
		cfg.skip_line( );

	for ( u32 i = start_char; i < 256; ++i )
	{
		u32 char_width;
		cfg.read_mask( TEXT_READER_MASK( "Char %*[0-9] Base Width,%d\n" ), &char_width );
		chars_width[i - start_char] = (u8)char_width;
	}

	enum { max_font_config = 2 * Kb };

	pvoid const output_data = stack_allocate( max_font_config );
	lib::writer output_cfg( output_data, max_font_config );

	output_cfg.write_str( str256( "textures\\fonts\\" ) + file_name + ".dds" );
	
	output_cfg.write<u16>( (u16)image_width );
	output_cfg.write<u16>( (u16)image_height );

	u8 const chars_in_row = (u8)( image_width / cell_width );
	u8 const chars_in_column = (u8)( image_height / cell_height );

	output_cfg.write<u8>( chars_in_row );
	output_cfg.write<u8>( chars_in_column );

	output_cfg.write<u8>( (u8)font_height );
	output_cfg.write<u8>( (u8)start_char );

	u8 const char_count = 256 - start_char;

	output_cfg.write<u8>( char_count );

	output_cfg.write_data( chars_width, char_count );

	return save( output_data, output_cfg.ptr( ) - output_data, output_path );
}


bool compile_config_render_model_mesh( pstr const data, uptr const size, pcstr const file_name, pcstr const output_path )
{
	uptr const cfg_memory_size = 64 * Kb;
	utils::config_reader cfg;
	cfg.create( stack_allocate( cfg_memory_size ), cfg_memory_size );
	bool const parsed = cfg.parse( (pcstr)data );

	if ( !parsed )
	{
		LOG( "error in config file \"%s\"\nline %d, %d: %s\n", output_path,
			cfg.error_info( ).line_index, cfg.error_info( ).line_position, cfg.error_info( ).message );
		return false;
	}

	uptr const output_memory_size = 64 * Kb;
	pvoid const output_memory = stack_allocate( output_memory_size );
	lib::writer output( output_memory, output_memory_size );

	utils::config_reader_node const& root = cfg.root( );

	output.write( read_float3( root["aabb_min"] ) );
	output.write( read_float3( root["aabb_max"] ) );
	output.write_str( root["mesh"].as_string( ) );
	output.write_str( root["albedo"].as_string( ) );
	output.write_str( root["metal_rough"].as_string( ) );

	return save( output_memory, output.size( output_memory ), output_path );
}


bool compile_config_object( pstr const data, uptr const size, pcstr const file_name, pcstr const output_path )
{
	uptr const cfg_memory_size = 1 * Mb;
	utils::config_reader cfg;
	cfg.create( stack_allocate( cfg_memory_size ), cfg_memory_size );
	bool const parsed = cfg.parse( (pcstr)data );

	if ( !parsed )
	{
		LOG( "error in config file \"%s\"\nline %d, %d: %s\n", output_path,
			cfg.error_info( ).line_index, cfg.error_info( ).line_position, cfg.error_info( ).message );
		return false;
	}

	uptr const output_memory_size = 1 * Mb;
	pvoid const output_memory = stack_allocate( output_memory_size );
	lib::writer output( output_memory, output_memory_size );

	utils::config_reader_node const& render_node = cfg.root( )["render"];

	u32* const render_size = output.ptr( );
	output += sizeof(u32);
	pvoid const render_output = output.ptr( );

	ASSERT( render_node.valid( ) );

	{
		u16* const mesh_object_count = output.ptr( );
		output += sizeof(u16);

		u16 count = 0;

		{
			utils::config_reader_node const& mesh_node = render_node["meshes"];

			ASSERT( mesh_node.valid( ) );
			ASSERT( mesh_node.type( ) == utils::config_node_type_list );

			count += (u32)mesh_node.size( );

			for ( uptr i = 0; i < mesh_node.size( ); ++i )
			{
				utils::config_reader_node const& n = mesh_node[i];

				output.write_str( n["model"].as_string( ) );
				output.write( read_float4x3( n["transform"] ) );
			}
		}

		*mesh_object_count = count;
	}

	*render_size = (u32)output.size( render_output );

	return save( output_memory, output.size( output_memory ), output_path );
}


bool compile_config_scene( pstr const data, uptr const size, pcstr const file_name, pcstr const output_path )
{
	uptr const cfg_memory_size = 16 * Mb;
	std_allocator::scoped_memory const cfg_memory( cfg_memory_size );
	utils::config_reader cfg;
	cfg.create( cfg_memory, cfg_memory_size );
	bool const parsed = cfg.parse( (pcstr)data );

	if ( !parsed )
	{
		LOG( "error in config file \"%s\"\nline %d, %d: %s\n", output_path,
			cfg.error_info( ).line_index, cfg.error_info( ).line_position, cfg.error_info( ).message );
		return false;
	}

	uptr const output_memory_size = 16 * Mb;
	std_allocator::scoped_memory const output_memory( output_memory_size );
	lib::writer output( output_memory, output_memory_size );

	utils::config_reader_node const& render_node = cfg.root( )["render"];

	u32* const render_size = output.ptr( );
	output += sizeof(u32);
	pvoid const render_output = output.ptr( );

	ASSERT( render_node.valid( ) );

	{
		utils::config_reader_node const& mesh_node = render_node["static_meshes"];

		ASSERT( mesh_node.valid( ) );
		ASSERT( mesh_node.type( ) == utils::config_node_type_list );

		u32 const mesh_count = (u32)mesh_node.size( );
		output.write( mesh_count );

		std_allocator::scoped_memory const bvh_aabbs_memory( sizeof(math::aabb) * mesh_count );
		std_allocator::scoped_memory const bvh_handles_memory( sizeof(u32) * mesh_count );

		math::aabb* const bvh_aabbs = bvh_aabbs_memory;
		u32* const bvh_handles = bvh_handles_memory;

		for ( u32 i = 0; i < mesh_node.size( ); ++i )
		{
			utils::config_reader_node const& n = mesh_node[i];

			pcstr const model_path = n["model"].as_string( );
			math::float4x3 const transform = read_float4x3( n["transform"] );

			output.write_str( model_path );
			output.write( transform );

			math::aabb mesh_aabb;

			{
				sys::path const& model_source_path = resources::get_source_path_from_target_path( resources::get_resource_path( model_path ) );

				sys::file f;
				f.create( model_source_path.c_str( ), sys::file::open_read );
				if ( !f.is_valid( ) )
				{
					LOG( "can't find render model while compiling scene config: \"%\"", output_path );
					return false;
				}

				uptr const model_data_size = f.size( );
				std_allocator::scoped_memory const model_data_ptr( model_data_size + 1 );

				f.read( model_data_ptr, model_data_size );
				f.destroy( );

				pstr const model_data = model_data_ptr;
				model_data[model_data_size] = '\0';

				uptr const model_cfg_memory_size = 1 * Mb;
				std_allocator::scoped_memory const model_cfg_memory( model_cfg_memory_size );
				utils::config_reader model_cfg;
				model_cfg.create( model_cfg_memory, model_cfg_memory_size );
				bool const model_parsed = model_cfg.parse( (pcstr)model_data );

				if ( !model_parsed )
				{
					LOG( "error in config file \"%s\"\nline %d, %d: %s\n", model_source_path.c_str( ),
						model_cfg.error_info( ).line_index, model_cfg.error_info( ).line_position, model_cfg.error_info( ).message );
					return false;
				}

				mesh_aabb.set_min_max( read_float3( model_cfg.root( )["aabb_min"] ), read_float3( model_cfg.root( )["aabb_max"] ) );
			}

			mesh_aabb.modify( transform );

			bvh_aabbs[i] = mesh_aabb;
			bvh_handles[i] = i;
		}

		uptr const bvh_memory_size = math::bvh::node_data_size * 2 * mesh_count;
		std_allocator::scoped_memory const bvh_memory( bvh_memory_size );

		math::bvh static_mesh_bvh;
		static_mesh_bvh.create( bvh_memory, bvh_memory_size );
		static_mesh_bvh.build( bvh_aabbs, bvh_handles, mesh_count );
		static_mesh_bvh.serialize( output );
		static_mesh_bvh.destroy( );
	}

	*render_size = (u32)output.size( render_output );

	return save( output_memory, output.size( output_memory ), output_path );
}

} // namespace resource_compiler