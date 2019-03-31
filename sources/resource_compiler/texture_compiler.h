#ifndef GUARD_RESOURCE_COMPILER_TEXTURE_COMPILER_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_TEXTURE_COMPILER_H_INCLUDED

#include <types.h>

namespace resource_compiler {

class texture_compiler
{
public:
	void create( pcstr const tex_conv_path );

	void compile( u64 const relevant_date,
				  pcstr const input_file_name,
				  pcstr const output_directory );

private:
	pcstr m_tex_conv_path;

};

} // namespace resource_compiler

#endif // #ifndef GUARD_RESOURCE_COMPILER_TEXTURE_COMPILER_H_INCLUDED