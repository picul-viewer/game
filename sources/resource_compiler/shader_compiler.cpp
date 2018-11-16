#include "shader_compiler.h"

#include <lib/allocator.h>
#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include <render/shader_containers.h>

#include <d3dcompiler.h>

namespace resource_compiler {

void shader_compiler::create( weak_const_string const version, bool const debug )
{
	m_version	= version;
	m_debug		= debug;
}

static inline bool verify_config_number( pcstr const shader_name, uptr const length, pcstr const shader_type_string )
{
	pcstr iter = shader_name + length - 1;

	for ( u8 j = 16; j; --j, --iter )
	{
		if ( ( ( *iter < '0' ) || ( *iter > '9' ) ) && ( ( *iter < 'A' ) || ( *iter > 'F' ) ) )
		{
			LOG( "shader_compiler: bad shader configuration defined for %s shader \"%s\"\n", shader_type_string, shader_name );
			return false;
		}
	}

	// Verify underscore symbol before configuration number
	if ( *iter != '_' )
	{
		LOG( "shader_compiler: bad shader configuration ( missing '_' ) defined for %s shader \"%s\"\n", shader_type_string, shader_name );
		return false;
	}

	return true;
}

template<typename PathProvider, typename ShaderType>
static inline void compile_shaders( weak_const_string const input_directory,
									weak_const_string const output_directory,
									uptr const shader_count,
									pcstr const shader_type,
									pcstr const file_extension,
									pcstr const shader_target,
									pcstr const version,
									bool const is_debug )
{
	little_string const common_output_folder = little_string( is_debug ? "debug_" : "release_" ) + version;

	{
		sys::path vertex_output_path( output_directory ); 
		vertex_output_path += common_output_folder;
		vertex_output_path += shader_type;
		
		vertex_output_path.create_directory( );

		for ( u32 i = 0; i < shader_count; ++i )
		{
			weak_const_string const shader_name = PathProvider::get( (ShaderType)i );
			uptr const shader_name_length = shader_name.length( );

			if ( !verify_config_number( shader_name, shader_name_length, shader_type ) )
				continue;

			little_string const shader_file_name = little_string( shader_name ).copy( 0, shader_name_length - 17 ) + file_extension;

			sys::path input_path( input_directory );
			input_path += shader_file_name;

			sys::file input( input_path.c_str( ), sys::file::open_read );
			if ( !input.is_valid( ) )
			{
				LOG( "shader_compiler: unable to open source file for %s shader: \"%s\"\n", shader_type, shader_name );
				continue;
			}

			uptr const input_file_size = input.size( );

			pvoid const data = stack_allocate( input_file_size );
			input.read( data, input_file_size );
			input.close( );

			little_string const config = little_string( "0x" ) + (pcstr)( shader_name - 16 );
			D3D_SHADER_MACRO macros[2];
			macros[0].Name = "CONFIGURATION";
			macros[0].Definition = config;
			macros[1].Name = nullptr;
			macros[1].Definition = nullptr;

			u32 const flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR |
				( is_debug ? ( D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION ) : D3DCOMPILE_OPTIMIZATION_LEVEL3 );

			ID3DBlob* result_code = nullptr;
			ID3DBlob* result_errors = nullptr;

			HRESULT const result = D3DCompile( data, input_file_size, input_path.c_str( ), macros,
											   D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
											   shader_target, flags, 0, &result_code, &result_errors );

			if ( SUCCEEDED( result ) )
			{
				if ( result_errors )
				{
					LOG( "shader_compiler: warnings occured when compiling %s shader: \"%s\"\n"
						 "%s"
						 "-----------------------------------------------------------------------------------\n",
						 shader_type, shader_name, (pcstr)result_errors->GetBufferPointer( ) );

					result_errors->Release( );
				}
				else
					LOG( "shader_compiler: %s shader (%s) compiled successfully: \"%s\"\n", shader_type, common_output_folder.c_str( ), shader_name );

				ASSERT( result_code );

				sys::path const output_path = vertex_output_path + shader_name;

				sys::file output( output_path.c_str( ), sys::file::open_write );
				
				output.write( result_code->GetBufferPointer( ), result_code->GetBufferSize( ) );
				output.close( );
				
				result_code->Release( );
			}
			else
			{
				if ( result_errors )
				{
					LOG( "shader_compiler: errors occured when compiling %s shader: \"%s\"\n"
						 "%s"
						 "-----------------------------------------------------------------------------------\n",
						 shader_type, shader_name, (pcstr)result_errors->GetBufferPointer( ) );
					
					result_errors->Release( );
				}
				else
					LOG( "shader_compiler: unknown error when compiling %s shader: \"%s\"\n", shader_type, shader_name );
				
				if ( result_code )
					result_code->Release( );
			}
		}
	}
}

void shader_compiler::compile( weak_const_string const input_directory, weak_const_string const output_directory )
{
	using namespace render::__render_shader_containers_detail;

	compile_shaders<vertex_shader_path_provider, render::vertex_shader_type>(
		input_directory, output_directory, render::vertex_shader_type_count,
		"vertex", ".vs", little_string( "vs_" ) + m_version, m_version, m_debug );

	compile_shaders<pixel_shader_path_provider, render::pixel_shader_type>(
		input_directory, output_directory, render::pixel_shader_type_count,
		"pixel", ".ps", little_string( "ps_" ) + m_version, m_version, m_debug );
	
	compile_shaders<geometry_shader_path_provider, render::geometry_shader_type>(
		input_directory, output_directory, render::geometry_shader_type_count,
		"geometry", ".gs", little_string( "gs_" ) + m_version, m_version, m_debug );
	
	compile_shaders<hull_shader_path_provider, render::hull_shader_type>(
		input_directory, output_directory, render::hull_shader_type_count,
		"hull", ".hs", little_string( "hs_" ) + m_version, m_version, m_debug );
	
	compile_shaders<domain_shader_path_provider, render::domain_shader_type>(
		input_directory, output_directory, render::domain_shader_type_count,
		"domain", ".ds", little_string( "ds_" ) + m_version, m_version, m_debug );
	
	compile_shaders<compute_shader_path_provider, render::compute_shader_type>(
		input_directory, output_directory, render::compute_shader_type_count,
		"compute", ".cs", little_string( "cs_" ) + m_version, m_version, m_debug );
}

} // namespace resource_compiler