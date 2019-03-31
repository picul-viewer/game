#include "shader_compiler.h"

#include <lib/allocator.h>
#include <lib/fixed_string.h>
#include <lib/linear_allocator.h>
#include <lib/weak_string.h>
#include <lib/writer.h>

#include <render/shader_containers.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace render;

#pragma warning( push )
#pragma warning( disable : 4065 )

struct vertex_shader_data_provider
{
	static pcstr get( vertex_shader_type type, u64& parameters )
	{
		switch ( type )
		{
		case vertex_shader_forward_default:
		{
			vertex_type const vtype = vertex_type_mesh;

			parameters = vtype;

			return "forward_default.vs";
		}
		case vertex_shader_ui_default:
		{
			vertex_type const vtype = vertex_type_ui;

			parameters = vtype;

			return "ui_default.vs";
		}
		default:
			UNREACHABLE_CODE
		}
	
		return nullptr;
	}
};

struct pixel_shader_data_provider
{
	static pcstr get( pixel_shader_type type, u64& parameters )
	{
		switch ( type )
		{
		case pixel_shader_forward_default:
		{
			vertex_type const vtype = vertex_type_mesh;

			parameters = vtype;

			return "forward_default.ps";
		}
		case pixel_shader_ui_default:
		{
			vertex_type const vtype = vertex_type_ui;

			parameters = vtype;

			return "ui_default.ps";
		}
		default:
			UNREACHABLE_CODE
		}

		return nullptr;
	}
};

struct geometry_shader_data_provider
{
	static pcstr get( geometry_shader_type type, u64& parameters )
	{
		switch ( type )
		{
		default:
			UNREACHABLE_CODE
		}
	
		return nullptr;
	}
};

struct hull_shader_data_provider
{
	static pcstr get( hull_shader_type type, u64& parameters )
	{
		switch ( type )
		{
		default:
			UNREACHABLE_CODE
		}
	
		return nullptr;
	}
};

struct domain_shader_data_provider
{
	static pcstr get( domain_shader_type type, u64& parameters )
	{
		switch ( type )
		{
		default:
			UNREACHABLE_CODE
		}
	
		return nullptr;
	}
};

struct compute_shader_data_provider
{
	static pcstr get( compute_shader_type type, u64& parameters )
	{
		switch ( type )
		{
		default:
			UNREACHABLE_CODE
		}
	
		return nullptr;
	}
};

#pragma warning( pop )

namespace resource_compiler {

void shader_compiler::create( weak_const_string const version, bool const debug )
{
	m_version	= version;
	m_debug		= debug;
}

little_string shader_parameters_to_string( u64 parameters )
{
	little_string s = "0x0000000000000000";

	for ( uptr i = s.length() - 1; parameters; --i, parameters >>= 4 )
	{
		u8 const char_value = parameters & 0xF;
		s[i] = ( char_value < 10 ) ? ( '0' + char_value ) : ( 'A' + char_value - 10 );
	}

	return s;
}

template<typename ShaderDataProvider, typename ShaderType, bool WriteVertexType>
static inline void compile_shader(	weak_const_string const input_directory,
									weak_const_string const shaders_mode,
									uptr const index,
									pcstr const shader_type,
									pcstr const shader_target,
									pcstr const version,
									bool const is_debug,
									dynamic_linear_allocator<Memory_Page_Size, 16384>& shader_data )
{
	u64 parameters = 0;
	weak_const_string const shader_name = ShaderDataProvider::get( (ShaderType)index, parameters );
	uptr const shader_name_length = shader_name.length( );

	sys::path input_path( input_directory );
	input_path += shader_name;

	sys::file input( input_path.c_str( ), sys::file::open_read );
	if ( !input.is_valid( ) )
	{
		LOG( "shader_compiler: unable to open source file for %s shader: \"%s\"\n", shader_type, shader_name );
		return;
	}

	uptr const input_file_size = input.size( );

	pvoid const data = stack_allocate( input_file_size );
	input.read( data, input_file_size );
	input.close( );

	little_string const& config = shader_parameters_to_string( parameters );
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
			LOG( "shader_compiler: %s shader (%s) compiled successfully: \"%s\"\n", shader_type, shaders_mode.c_str( ), shader_name );

		ASSERT( result_code );

		pvoid const result_data = result_code->GetBufferPointer( );
		uptr const result_size_uptr = result_code->GetBufferSize( );
		ASSERT_CMP( result_size_uptr, <, 4 * Gb );
		u32 const result_size = (u32)result_size_uptr;

		uptr const output_size = sizeof(u32) + result_size_uptr + ( WriteVertexType ? sizeof(u8) : 0 );
		pointer const output_ptr = shader_data.allocate( output_size );
			
		lib::writer w( output_ptr, output_size );
		w.write( result_size );

		if ( WriteVertexType )
			w.write( (u8)( parameters & 0xFF ) );

		w.write_data( result_data, result_size_uptr );

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

template<typename ShaderDataProvider, typename ShaderType, bool WriteVertexType>
static inline void compile_shaders( weak_const_string const input_directory,
									weak_const_string const shaders_mode,
									uptr const shader_count,
									pcstr const shader_type,
									pcstr const shader_target,
									pcstr const version,
									bool const is_debug,
									dynamic_linear_allocator<Memory_Page_Size, 16384>& shader_data )
{
	for ( u32 i = 0; i < shader_count; ++i )
	{
		compile_shader<ShaderDataProvider, ShaderType, WriteVertexType>( input_directory, shaders_mode, i, shader_type, shader_target, version, is_debug, shader_data );
	}
}

void shader_compiler::compile( weak_const_string const input_directory, weak_const_string const output_directory )
{
	dynamic_linear_allocator<Memory_Page_Size, 16384> shader_data;
	shader_data.create( );
	
	little_string const& shaders_mode = little_string( m_debug ? "debug_" : "release_" ) + m_version;

	compile_shaders<vertex_shader_data_provider, vertex_shader_type, true>(
		input_directory, shaders_mode, vertex_shader_type_count, "vertex",
		little_string( "vs_" ) + m_version, m_version, m_debug, shader_data );

	compile_shaders<pixel_shader_data_provider, pixel_shader_type, false>(
		input_directory, shaders_mode, pixel_shader_type_count, "pixel",
		little_string( "ps_" ) + m_version, m_version, m_debug, shader_data );
	
	compile_shaders<geometry_shader_data_provider, geometry_shader_type, false>(
		input_directory, shaders_mode, geometry_shader_type_count, "geometry",
		little_string( "gs_" ) + m_version, m_version, m_debug, shader_data );
	
	compile_shaders<hull_shader_data_provider, hull_shader_type, false>(
		input_directory, shaders_mode, hull_shader_type_count, "hull",
		little_string( "hs_" ) + m_version, m_version, m_debug, shader_data );
	
	compile_shaders<domain_shader_data_provider, domain_shader_type, false>(
		input_directory, shaders_mode, domain_shader_type_count, "domain",
		little_string( "ds_" ) + m_version, m_version, m_debug, shader_data );
	
	compile_shaders<compute_shader_data_provider, compute_shader_type, false>(
		input_directory, shaders_mode, compute_shader_type_count, "compute",
		little_string( "cs_" ) + m_version, m_version, m_debug, shader_data );
	
	sys::path output_path( output_directory );
	output_path.create_directory( );
	output_path += shaders_mode;
	
	sys::file f( output_path.c_str( ), sys::file::open_write );
	if ( !f.is_valid( ) )
	{
		LOG( "shader_compiler: unable to write compiled shaders" );
		return;
	}

	f.write( shader_data.data(), shader_data.data_end( ) - shader_data.data( ) );
	f.close( );

	shader_data.destroy( );
}

} // namespace resource_compiler