#include "renderer.h"

#include "api.h"

#include "resources.h"

#include "resource_views.h"
#include "render_targets.h"

#include "parameters.h"
#include "parameters_manager.h"

#include "render_object_allocator.h"

#include "scene.h"

#include "statistics.h"

namespace render {
	
renderer::renderer( ) :
	m_scene			( nullptr ),
	m_next_scene	( nullptr )
{ }

void renderer::create( )
{
	m_scene			= nullptr;
	m_next_scene	= nullptr;
	
	m_render_objects_memory = virtual_allocator( ).allocate( render_objects_memory_size );

	m_render_meshes.create(
		(render_object_mesh const**)( m_render_objects_memory + render_meshes_memory_offset ),
		max_render_meshes
	);

	g_api.create( );

	g_resources.create( );

	m_stage_initialization.create( );
	m_stage_visibility.create( );
	m_stage_forward_default.create( );
#ifdef USE_RENDER_PROFILING
	m_stage_statistics.create( );
#endif // #ifdef USE_RENDER_PROFILING
	m_stage_ui.create( );
}

void renderer::destroy( )
{
	m_stage_initialization.destroy( );
	m_stage_visibility.destroy( );
	m_stage_forward_default.destroy( );
#ifdef USE_RENDER_PROFILING
	m_stage_statistics.destroy( );
#endif // #ifdef USE_RENDER_PROFILING
	m_stage_ui.destroy( );
	
	g_resources.destroy( );

	g_api.destroy( );
	
	virtual_allocator( ).deallocate( m_render_objects_memory );
}

void renderer::render_scene( )
{
	m_stage_visibility.execute( );
	m_stage_forward_default.execute	( );
}

void renderer::render( )
{
	bool const is_rendering_scene =
		( m_scene != nullptr );

	{
		RENDER_FRAME;
		
		m_stage_initialization.execute	( );

		if ( is_rendering_scene )
			render_scene( );
	
#ifdef USE_RENDER_PROFILING
		m_stage_statistics.execute		( );
#endif // #ifdef USE_RENDER_PROFILING
		m_stage_ui.execute				( );

		g_api.get_swap_chain( )->Present( 0, 0 );
	}

	end_frame( );
}

void renderer::end_frame( )
{
	g_parameters_manager.update( );

	m_scene		= m_next_scene;
}

renderer g_renderer;

} // namespace render