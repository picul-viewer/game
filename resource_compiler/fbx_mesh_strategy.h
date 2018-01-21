#ifndef __resource_compiler_fbx_mesh_strategy_h_included_
#define __resource_compiler_fbx_mesh_strategy_h_included_

#include <core/types.h>
#include <core/std.h>

#include "resource_compiler.h"

#include <fbxsdk.h>

namespace resource_compiler {

class fbx_mesh_strategy : public strategy
{
public:
	virtual void on_start( ) override;
	virtual void on_finish( ) override;

	virtual void compile( u64 relevant_date,
						  weak_const_string input_path,
						  weak_const_string output_directory ) override;
protected:
	FbxManager*		m_fbx_manager;
	FbxImporter*	m_importer;
};

} // namespace resource_compiler

#endif // #ifndef __resource_compiler_fbx_mesh_strategy_h_included_