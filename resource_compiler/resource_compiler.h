#ifndef __resource_compiler_resource_compiler_h_included_
#define __resource_compiler_resource_compiler_h_included_

#include <core/types.h>
#include <core/std.h>

#include <vector>

namespace resource_compiler {

class strategy
{
public:
	virtual void on_start( ) { }
	virtual void on_finish( ) { }

	virtual void compile( u64 relevant_date,
						  weak_const_string input_path,
						  weak_const_string output_directory ) = 0;
};

struct category
{
	weak_const_string input_path;
	weak_const_string output_path;
	strategy* strategy;

	category( ) = default;
	category( weak_const_string input_path, weak_const_string output_path, resource_compiler::strategy* strategy );
};

inline u64 filetime_to_u64( FILETIME data );

class manager
{
public:
	manager( uptr category_count );
	~manager( );

	void add_category( strategy* strategy, weak_const_string input_path, weak_const_string output_path );

	void compile( weak_const_string input_path, weak_const_string output_path );

protected:
	void compile_category( category& category );

	std::vector<category> m_categories;
	weak_const_string m_input_path;
	weak_const_string m_output_path;
};

} // namespace resource_compiler

#endif // #ifndef __resource_compiler_resource_compiler_h_included_