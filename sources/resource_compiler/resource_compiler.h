#ifndef GUARD_RESOURCE_COMPILER_RESOURCE_COMPILER_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_RESOURCE_COMPILER_H_INCLUDED

#include <types.h>

#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include "obj_compiler.h"
#include "gltf_compiler.h"
#include "texture_compiler.h"
#include "config_compiler.h"

namespace resource_compiler {

class resource_compiler
{
public:
	void create( );
	void destroy( );

	void compile( weak_const_string const input_path, weak_const_string const output_path );

public:
	typedef void ( resource_compiler::*scan_functor )( u64 const, pcstr const, pcstr const );

	void scan(
		uptr const input_path_size, str512& input_path,
		uptr const output_path_size, str512& output_path,
		uptr const filename_ending_size, weak_const_string const filename_ending,
		scan_functor const functor );

protected:
	void compile_obj( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory );
	void compile_gltf( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory );
	void compile_texture( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory );
	void compile_config( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory );

protected:
	obj_compiler m_obj_compiler;

	gltf_compiler m_gltf_compiler;

	texture_compiler m_texture_compiler;

	config_compiler m_config_compiler;

};

} // namespace resource_compiler

#endif // #ifndef GUARD_RESOURCE_COMPILER_RESOURCE_COMPILER_H_INCLUDED