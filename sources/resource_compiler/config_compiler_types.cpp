#include <types.h>
#include <lib/allocator.h>
#include <lib/binary_config.h>
#include <lib/text_config.h>
#include <system/file.h>
#include <utils/resources_path.h>

namespace resource_compiler {

static bool save( pvoid const data, uptr const size, pcstr const output_path )
{
	sys::file f( output_path, sys::file::open_write );
	if ( !f.is_valid( ) )
		return false;

	f.write( data, size );
	f.close( );

	return true;
}

bool compile_config_font( pvoid const data, uptr const size, pcstr const file_name, pcstr const output_path )
{
	text_config cfg( data );

	u32 image_width;
	u32 image_height;
	u32 cell_width;
	u32 cell_height;
	u32 start_char;
	u32 font_height;

	u8 chars_width[256];

	cfg.read_mask( TEXT_CONFIG_MASK( "Image Width,%d\n" ), &image_width );
	cfg.read_mask( TEXT_CONFIG_MASK( "Image Height,%d\n" ), &image_height );
	cfg.read_mask( TEXT_CONFIG_MASK( "Cell Width,%d\n" ), &cell_width );
	cfg.read_mask( TEXT_CONFIG_MASK( "Cell Height,%d\n" ), &cell_height );
	cfg.read_mask( TEXT_CONFIG_MASK( "Start Char,%d\n" ), &start_char );
	cfg.skip_line( );
	cfg.read_mask( TEXT_CONFIG_MASK( "Font Height,%d\n" ), &font_height );
	cfg.skip_line( );

	for ( u32 i = 0; i < start_char; ++i )
		cfg.skip_line( );

	for ( u32 i = start_char; i < 256; ++i )
	{
		u32 char_width;
		cfg.read_mask( TEXT_CONFIG_MASK( "Char %*[0-9] Base Width,%d\n" ), &char_width );
		chars_width[i - start_char] = (u8)char_width;
	}

	enum { max_font_config = 2 * Kb };

	pvoid const output_data = stack_allocate( max_font_config );
	binary_config output_cfg( output_data, max_font_config );

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

	return save( output_data, output_cfg.get_pointer( ) - output_data, output_path );
}

} // namespace resource_compiler