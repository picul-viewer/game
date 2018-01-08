#ifndef __resource_compiler_resource_compiler_h_included_
#define __resource_compiler_resource_compiler_h_included_

#include <core/types.h>
#include <core/std.h>

namespace resource_compiler {

class strategy
{
public:
	virtual void on_start( ) { }
	virtual void on_finish( ) { }

	virtual void get_output_name( weak_const_string input_name, weak_string output_name ) = 0;
	virtual void compile( weak_const_string input_path,
						  weak_const_string output_directory ) = 0;
};

struct category
{
	weak_const_string path;
	strategy* strategy;
};

class manager
{
public:
	manager( std::initializer_list<category> const& init );
	~manager( );

	void compile( weak_const_string input_path, weak_const_string output_path );

protected:
	void compile_category( category& category );

	std::vector<category> m_categories;
	weak_const_string m_input_path;
	weak_const_string m_output_path;
};

} // namespace resource_compiler

#endif // #ifndef __resource_compiler_resource_compiler_h_included_