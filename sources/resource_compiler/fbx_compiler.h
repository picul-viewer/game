#ifndef GUARD_RESOURCE_COMPILER_FBX_COMPILER_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_FBX_COMPILER_H_INCLUDED

#include <types.h>
#include <lib/weak_string.h>

#include <fbxsdk.h>

namespace resource_compiler {

class fbx_compiler
{
public:
	void create( );
	void destroy( );

	void compile( u64 const relevant_date,
				  weak_const_string const input_file_name,
				  weak_const_string const output_directory );
protected:
	FbxManager*		m_fbx_manager;
	FbxImporter*	m_importer;
};

} // namespace resource_compiler

#endif // #ifndef GUARD_RESOURCE_COMPILER_FBX_COMPILER_H_INCLUDED