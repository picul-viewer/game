#ifndef __resource_compiler_resource_compiler_h_included_
#define __resource_compiler_resource_compiler_h_included_

#include <types.h>

#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include "shader_compiler.h"
#include "fbx_compiler.h"

namespace resource_compiler {

class resource_compiler
{
public:
	void create( );
	void destroy( );

	void compile( weak_const_string input_path, weak_const_string output_path );

public:
	typedef void ( resource_compiler::*scan_functor )( u64, pcstr, pcstr );

	void scan(
		uptr input_path_size, str512& input_path,
		uptr output_path_size, str512& output_path,
		uptr filename_ending_size, weak_const_string filename_ending,
		scan_functor functor );

protected:
	void compile_fbx( u64 relevant_date, pcstr input_file_name, pcstr output_directory );

protected:
	shader_compiler m_shader_compiler_dbg_4_0;
	shader_compiler m_shader_compiler_rel_4_0;
	shader_compiler m_shader_compiler_dbg_5_0;
	shader_compiler m_shader_compiler_rel_5_0;

	fbx_compiler m_fbx_compiler;

};

} // namespace resource_compiler

#endif // #ifndef __resource_compiler_resource_compiler_h_included_