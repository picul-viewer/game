#ifndef GUARD_RESOURCE_COMPILER_RESOURCE_COMPILER_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_RESOURCE_COMPILER_H_INCLUDED

#include <types.h>

#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include "shader_compiler.h"
#include "fbx_compiler.h"
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
	void compile_fbx( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory );
	void compile_texture( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory );
	void compile_config( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory );

protected:
	shader_compiler m_shader_compiler_dbg_4_0;
	shader_compiler m_shader_compiler_rel_4_0;
	shader_compiler m_shader_compiler_dbg_5_0;
	shader_compiler m_shader_compiler_rel_5_0;

	fbx_compiler m_fbx_compiler;

	texture_compiler m_texture_compiler;

	config_compiler m_config_compiler;

};

} // namespace resource_compiler

#endif // #ifndef GUARD_RESOURCE_COMPILER_RESOURCE_COMPILER_H_INCLUDED