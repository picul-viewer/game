#ifndef __resource_compiler_shader_strategy_h_included_
#define __resource_compiler_shader_strategy_h_included_

#include "resource_compiler.h"

namespace resource_compiler {

class shader_strategy : public strategy
{
public:
	shader_strategy( weak_const_string version, bool debug );

	virtual void compile( u64 relevant_date,
						  weak_const_string input_path,
						  weak_const_string output_directory ) override;
protected:
	weak_const_string	m_version;
	bool				m_debug;

	pointer execute( weak_string command_line );
};

} // namespace resource_compiler

#endif // #ifndef __resource_compiler_shader_strategy_h_included_