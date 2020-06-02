#include "resource_compiler.h"

#include <system/path.h>
#include <system/thread.h>
#include <system/file_iterator.h>

#include <Shlwapi.h>

namespace resource_compiler {

void resource_compiler::create( )
{
	m_obj_compiler.create( );

	m_gltf_compiler.create( );

	pcstr const tex_conv_path = "..\\..\\prebuilt\\texconv.exe";
	m_texture_compiler.create( tex_conv_path );
	
	m_config_compiler.create( );
}

void resource_compiler::destroy( )
{
	m_obj_compiler.destroy( );
	m_gltf_compiler.destroy( );
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
		obj_compiler,

		gltf_compiler,

		texture_compiler,

		config_compiler,

		threads_count
	};

	sys::thread threads[threads_count];

	u32 thread_index = 0;

	scan_execute_data obj_thread_data;
	
	sys::path obj_input;
	sys::path obj_output;

	{
		obj_input = input_path;
		obj_input += "scenes";
		
		obj_output = output_path;
		obj_output += "configs/render/scenes";

		obj_output.create_directory( );

		obj_thread_data = { this, &resource_compiler::scan, obj_input.c_str( ), obj_output.c_str( ), ".obj", &resource_compiler::compile_obj };

		threads[thread_index].create( sys::thread::func_helper<&scan_thread_function>, 16 * Mb, &obj_thread_data );
		++thread_index;
	}

	scan_execute_data gltf_thread_data;
	
	sys::path gltf_input;
	sys::path gltf_output;

	{
		gltf_input = input_path;
		gltf_input += "scenes";
		
		gltf_output = output_path;
		gltf_output += "configs/render/scenes";

		gltf_output.create_directory( );

		gltf_thread_data = { this, &resource_compiler::scan, gltf_input.c_str( ), gltf_output.c_str( ), ".gltf", &resource_compiler::compile_gltf };

		threads[thread_index].create( sys::thread::func_helper<&scan_thread_function>, 16 * Mb, &gltf_thread_data );
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

		threads[thread_index].create( sys::thread::func_helper<&scan_thread_function>, 1 * Mb, &texture_thread_data );
		++thread_index;
	}
	
	scan_execute_data config_thread_data;
	
	sys::path config_input;
	sys::path config_output;

	{
		config_input = input_path;
		config_input += "configs";

		config_output = output_path;
		config_output += "configs";

		config_output.create_directory( );

		config_thread_data = { this, &resource_compiler::scan, config_input.c_str( ), config_output.c_str( ), ".cfg", &resource_compiler::compile_config };

		threads[thread_index].create( sys::thread::func_helper<&scan_thread_function>, 1 * Mb, &config_thread_data );
		++thread_index;
	}

	ASSERT_CMP( thread_index, ==, threads_count );

	sys::thread::destroy( threads_count, threads );
}

void resource_compiler::scan(
	uptr const input_path_size, str512& input_path,
	uptr const output_path_size, str512& output_path,
	uptr const filename_ending_size, weak_const_string const filename_ending,
	scan_functor const functor )
{
	if ( !sys::path::is_valid( output_path ) )
		sys::path::create_directory( output_path );
	
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

void resource_compiler::compile_obj( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	m_obj_compiler.compile( relevant_date, input_file_name, output_directory );
}

void resource_compiler::compile_gltf( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	m_gltf_compiler.compile( relevant_date, input_file_name, output_directory );
}

void resource_compiler::compile_texture( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	m_texture_compiler.compile( relevant_date, input_file_name, output_directory );
}

void resource_compiler::compile_config( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	m_config_compiler.compile( relevant_date, input_file_name, output_directory );
}

} // namespace resource_compiler