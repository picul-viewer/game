#include "resource_compiler.h"

#include <system/path.h>
#include <system/thread.h>

#include <Shlwapi.h>

namespace resource_compiler {

inline u64 filetime_to_u64( FILETIME data )
{
	return *(u64*)&data;
}

void resource_compiler::create( )
{
	m_shader_compiler_dbg_4_0.create( "4_0", true );
	m_shader_compiler_rel_4_0.create( "4_0", false );
	m_shader_compiler_dbg_5_0.create( "5_0", true );
	m_shader_compiler_rel_5_0.create( "5_0", false );

	m_fbx_compiler.create( );
}

void resource_compiler::destroy( )
{
	m_fbx_compiler.destroy( );
}

void resource_compiler::compile( weak_const_string input_path, weak_const_string output_path )
{
	enum resource_compiler_threads
	{
		shader_compiler_debug_4_0 = 0,
		shader_compiler_release_4_0,
		shader_compiler_debug_5_0,
		shader_compiler_release_5_0,

		fbx_compiler,

		threads_count
	};

	thread threads[threads_count];

	u32 thread_index = 0;

	struct shader_compiler_data
	{
		shader_compiler* compiler;
		pcstr input_path;
		pcstr output_path;
	} shader_thread_data[4];

	{
		sys::path input( input_path );
		input += "shaders";
		
		sys::path output( output_path );
		output += "shaders";

		output.create_directory( );

		auto thread_function = []( void* param )
		{
			shader_compiler_data* data = (shader_compiler_data*)param;

			data->compiler->compile( data->input_path, data->output_path );
		};

		shader_thread_data[0] = { &m_shader_compiler_dbg_4_0, input.c_str( ), output.c_str( ) };
		shader_thread_data[1] = { &m_shader_compiler_rel_4_0, input.c_str( ), output.c_str( ) };
		shader_thread_data[2] = { &m_shader_compiler_dbg_5_0, input.c_str( ), output.c_str( ) };
		shader_thread_data[3] = { &m_shader_compiler_rel_5_0, input.c_str( ), output.c_str( ) };

		for ( u32 i = 0; i < 4; ++i )
		{
			threads[thread_index].create( thread_function, 1 * Mb, &shader_thread_data[i] );
			++thread_index;
		}
	}

	typedef decltype(&resource_compiler::scan) scan_decl;

	struct scan_execute_data
	{
		resource_compiler* this_ptr;
		scan_decl this_scan_functor;
		weak_const_string input_path;
		weak_const_string output_path;
		weak_const_string extension;
		scan_functor functor;
	};

	auto scan_thread_function = []( void* param )
	{
		scan_execute_data* data = (scan_execute_data*)param;
		scan_decl scan = data->this_scan_functor;
		scan_functor functor = data->functor;

		str512 input = data->input_path;
		str512 output = data->output_path;

		( data->this_ptr->*scan )( input.length( ), input,
								   output.length( ), output,
								   data->extension.length( ), data->extension,
								   functor );
	};

	scan_execute_data fbx_thread_data;

	{
		sys::path input( input_path );
		input += "meshes";
		
		sys::path output( output_path );
		output += "meshes";

		output.create_directory( );

		fbx_thread_data = { this, &resource_compiler::scan, input.c_str( ), output.c_str( ), ".fbx", &resource_compiler::compile_fbx };

		threads[thread_index].create( scan_thread_function, 1 * Mb, &fbx_thread_data );
		++thread_index;
	}

	ASSERT( thread_index == threads_count );

	threads[0].destroy( INFINITE, threads_count );
}

void resource_compiler::scan( uptr input_path_size, str512& input_path, uptr output_path_size, str512& output_path, uptr filename_ending_size, weak_const_string filename_ending, scan_functor functor )
{
	if ( !sys::path::is_valid( output_path ) )
		return;
	
	// creating mask
	input_path += "/*";

	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile( input_path.c_str( ), &file_data );
	
	input_path.resize( input_path_size );

	if ( file_handle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( strings::equal( file_data.cFileName, "." ) ||
				 strings::equal( file_data.cFileName, ".." ) )
				continue;

			uptr length = strings::length( file_data.cFileName );

			if ( file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				input_path += "/";
				input_path += file_data.cFileName;

				output_path += "/";
				output_path += file_data.cFileName;
								
				scan( input_path_size + length + 1, input_path, output_path_size + length + 1, output_path, filename_ending_size, filename_ending, functor );

				input_path.resize( input_path_size );
				output_path.resize( output_path_size );
			}
			else if ( strings::equal( file_data.cFileName + length - filename_ending_size, filename_ending)  )
			{
				input_path += "/";
				input_path += file_data.cFileName;

				( this->*functor )( filetime_to_u64( file_data.ftLastWriteTime ), input_path, output_path );
				
				input_path.resize( input_path_size );
			}
		}
		while ( FindNextFile( file_handle, &file_data ) );

		FindClose( file_handle );
	}
}

void resource_compiler::compile_fbx( u64 relevant_date, pcstr input_file_name, pcstr output_directory )
{
	m_fbx_compiler.compile( relevant_date, input_file_name, output_directory );
}

} // namespace resource_compiler