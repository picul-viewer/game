#ifndef __resource_compiler_config_compiler_h_included_
#define __resource_compiler_config_compiler_h_included_

#include <types.h>
#include <lib/weak_string.h>

namespace resource_compiler {

class config_compiler
{
public:
	void create( );

	void compile( u64 const relevant_date,
				  pcstr const input_file_name,
				  pcstr const output_directory );

};

} // namespace resource_compiler

#endif // #ifndef __resource_compiler_shader_compiler_h_included_