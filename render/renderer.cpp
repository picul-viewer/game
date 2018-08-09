#include "renderer.h"

#include "render_api.h"

#include "resources.h"

#include "resource_views.h"
#include "render_targets.h"

#include "render_parameters.h"

#include "render_object_allocator.h"

#include "scene.h"

#include "render_parameters_manager.h"

namespace render {

void renderer::create( )
{
	m_scene			= nullptr;
	m_next_scene	= nullptr;
	
	m_camera		= nullptr;
	m_next_camera	= nullptr;

	m_render_objects_memory = virtual_mem_allocator( ).allocate( render_objects_memory_size );

	m_render_meshes.set(
		(render_object_mesh const**)( m_render_objects_memory + render_meshes_memory_offset ),
		max_render_meshes
	);

	g_api.create( );

	g_resources.create( );

	m_stage_initialization.create( );
	m_stage_forward_default.create( );
}

void renderer::destroy( )
{
	m_stage_initialization.destroy( );
	m_stage_forward_default.destroy( );
	
	g_resources.destroy( );

	g_api.destroy( );
	
	virtual_mem_allocator( ).deallocate( m_render_objects_memory );
}

void renderer::render_scene( )
{
	m_stage_initialization.execute	( );
	m_stage_forward_default.execute	( );
}

void renderer::render( )
{
	bool const is_rendering_scene =
		( m_scene != nullptr ) &&
		( m_camera != nullptr );

	if ( is_rendering_scene )
		render_scene( );

	end_frame( );
}

void renderer::end_frame( )
{
	g_parameters_manager.update( );

	m_scene		= m_next_scene;
	m_camera	= m_next_camera;
}

renderer g_renderer;

} // namespace render