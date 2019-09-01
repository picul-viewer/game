#ifndef GUARD_RENDER_MESH_COOK_H_INCLUDED
#define GUARD_RENDER_MESH_COOK_H_INCLUDED

#include <types.h>

#include <resource_system/queried_resources.h>
#include <resource_system/shared_resource_cook.h>
#include <utils/engine_threads.h>

namespace render {

class mesh;

class mesh_cook : public shared_resource_cook<mesh, mesh_cook>
{
public:
	static mesh_cook* create( pcstr const in_path );
	static void destroy( pointer const in_cook );

public:
	enum : u32 {
		create_thread_index = engine_thread_main
	};

	void create_resource( );

private:
	void query_file( mesh* const in_resource_ptr );
	void on_file_loaded( queried_resources& in_queried );

private:
	mesh* m_result;
	uptr m_length;

#pragma warning (push)
#pragma warning (disable:4200)
	char m_path[0];
#pragma warning (pop)

};

} // namespace render

#endif // #ifndef GUARD_RENDER_MESH_COOK_H_INCLUDED