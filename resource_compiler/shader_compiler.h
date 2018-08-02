#ifndef __resource_compiler_shader_compiler_h_included_
#define __resource_compiler_shader_compiler_h_included_

#include <lib/weak_string.h>

namespace resource_compiler {

class shader_compiler
{
public:
	void create( weak_const_string version, bool debug );

	void compile( weak_const_string input_directory, weak_const_string output_directory );

protected:
	weak_const_string	m_version;
	bool				m_debug;
};

} // namespace resource_compiler

#endif // #ifndef __resource_compiler_shader_compiler_h_included_