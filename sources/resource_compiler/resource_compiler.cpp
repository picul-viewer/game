#include "resource_compiler.h"

#include <system/path.h>
#include <system/thread.h>
#include <system/file_iterator.h>

#include <Shlwapi.h>

namespace resource_compiler {

void resource_compiler::create( int argc, char** argv )
{
	m_shader_compiler_dbg_4_0.create( "4_0", true );
	m_shader_compiler_rel_4_0.create( "4_0", false );
	m_shader_compiler_dbg_5_0.create( "5_0", true );
	m_shader_compiler_rel_5_0.create( "5_0", false );

	m_fbx_compiler.create( );

	ASSERT( argc >= 2 );
	m_texture_compiler.create( argv[1] );
}

void resource_compiler::destroy( )
{
	m_fbx_compiler.destroy( );
}

struct shader_compiler_data
{
	shader_compiler* compiler;
	pcstr input_path;
	pcstr output_path;
};

void shader_compiler_thread_function( void* const param )
{
	shader_compiler_data* data = (shader_compiler_data*)param;

	data->compiler->compile( data->input_path, data->output_path );
}

typedef decltype(&resource_compiler::scan) scan_decl;

struct scan_execute_data
{
	resource_compiler* this_ptr;
	scan_decl this_scan_functor;
	weak_const_string input_path;
	weak_const_string output_path;
	weak_const_string extension;
	resource_compiler::scan_functor functor;
};

void scan_thread_function( void* const param )
{
	scan_execute_data* data = (scan_execute_data*)param;
	scan_decl scan = data->this_scan_functor;
	resource_compiler::scan_functor functor = data->functor;

	str512 input = data->input_path;
	str512 output = data->output_path;

	( data->this_ptr->*scan )( input.length( ), input,
								output.length( ), output,
								data->extension.length( ), data->extension,
								functor );
};

void resource_compiler::compile( weak_const_string const input_path, weak_const_string const output_path )
{
	enum resource_compiler_threads
	{
		shader_compiler_debug_4_0 = 0,
		shader_compiler_release_4_0,
		shader_compiler_debug_5_0,
		shader_compiler_release_5_0,

		fbx_compiler,

		texture_compiler,

		threads_count
	};

	thread threads[threads_count];

	u32 thread_index = 0;

	shader_compiler_data shader_thread_data[4];

	sys::path shader_input;
	sys::path shader_output;

	{
		shader_input = input_path;
		shader_input += "shaders";
		
		shader_output = output_path;
		shader_output += "shaders";

		shader_output.create_directory( );

		shader_thread_data[0].compiler = &m_shader_compiler_dbg_4_0;
		shader_thread_data[0].input_path = shader_input.c_str( );
		shader_thread_data[0].output_path = shader_output.c_str( );

		shader_thread_data[1].compiler = &m_shader_compiler_rel_4_0;
		shader_thread_data[1].input_path = shader_input.c_str( );
		shader_thread_data[1].output_path = shader_output.c_str( );

		shader_thread_data[2].compiler = &m_shader_compiler_dbg_5_0;
		shader_thread_data[2].input_path = shader_input.c_str( );
		shader_thread_data[2].output_path = shader_output.c_str( );

		shader_thread_data[3].compiler = &m_shader_compiler_rel_5_0;
		shader_thread_data[3].input_path = shader_input.c_str( );
		shader_thread_data[3].output_path = shader_output.c_str( );

		for ( u32 i = 0; i < 4; ++i )
		{
			threads[thread_index].create( thread::func_helper<&shader_compiler_thread_function>, 1 * Mb, &shader_thread_data[i] );
			++thread_index;
		}
	}

	scan_execute_data fbx_thread_data;
	
	sys::path fbx_input;
	sys::path fbx_output;

	{
		fbx_input = input_path;
		fbx_input += "meshes";
		
		fbx_output = output_path;
		fbx_output += "meshes";

		fbx_output.create_directory( );

		fbx_thread_data = { this, &resource_compiler::scan, fbx_input.c_str( ), fbx_output.c_str( ), ".fbx", &resource_compiler::compile_fbx };

		threads[thread_index].create( thread::func_helper<&scan_thread_function>, 1 * Mb, &fbx_thread_data );
		++thread_index;
	}
	
	scan_execute_data texture_thread_data;
	
	sys::path texture_input;
	sys::path texture_output;

	{
		texture_input = input_path;
		texture_input += "textures";

		texture_output = output_path;
		texture_output += "textures";

		texture_output.create_directory( );

		texture_thread_data = { this, &resource_compiler::scan, texture_input.c_str( ), texture_output.c_str( ), "", &resource_compiler::compile_texture };

		threads[thread_index].create( thread::func_helper<&scan_thread_function>, 1 * Mb, &texture_thread_data );
		++thread_index;
	}

	ASSERT( thread_index == threads_count );

	threads->destroy( INFINITE, threads_count );
}

void resource_compiler::scan(
	uptr const input_path_size, str512& input_path,
	uptr const output_path_size, str512& output_path,
	uptr const filename_ending_size, weak_const_string const filename_ending,
	scan_functor const functor )
{
	if ( !sys::path::is_valid( output_path ) )
		return;
	
	// creating mask
	input_path += "\\*";

	sys::file_iterator fi;
	fi.create( input_path );

	input_path.resize( input_path_size );

	if ( fi.is_valid( ) )
	{
		do
		{
			uptr const length = strings::length( fi.get_file_name( ) );

			if ( fi.is_directory( ) )
			{
				input_path += "\\";
				input_path += fi.get_file_name( );

				output_path += "\\";
				output_path += fi.get_file_name( );
								
				scan( input_path_size + length + 1, input_path, output_path_size + length + 1, output_path, filename_ending_size, filename_ending, functor );

				input_path.resize( input_path_size );
				output_path.resize( output_path_size );
			}
			else if ( strings::equal( fi.get_file_name( ) + length - filename_ending_size, filename_ending)  )
			{
				input_path += "\\";
				input_path += fi.get_file_name( );

				( this->*functor )( fi.get_modification_time( ), input_path, output_path );
				
				input_path.resize( input_path_size );
			}
		}
		while ( fi.next( ) );

		fi.destroy( );
	}
}

void resource_compiler::compile_fbx( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	m_fbx_compiler.compile( relevant_date, input_file_name, output_directory );
}

void resource_compiler::compile_texture( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	m_texture_compiler.compile( relevant_date, input_file_name, output_directory );
}

} // namespace resource_compiler