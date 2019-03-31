#ifndef GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_H_INCLUDED

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

#endif // #ifndef GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_H_INCLUDED