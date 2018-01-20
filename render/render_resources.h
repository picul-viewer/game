#ifndef __render_render_resources_h_included_
#define __render_render_resources_h_included_

#include <core/types.h>
#include "dx_include.h"

#include "resource_pool.h"
#include "pipeline_state_pool.h"
#include "shader_pool.h"

#include "effect.h"

namespace render {

namespace resources
{
	void create( );
	void destroy( );
	
	typedef resource_pool<mesh>				mesh_pool;
	typedef resource_pool<texture>			texture_pool;

	mesh_pool&			get_mesh_pool( );
	texture_pool&		get_texture_pool( );

	typedef pipeline_state_pool<32, 16, 16>	pipeline_pool;

	pipeline_pool&		get_pipeline_pool( );
	
	shader_pool&		get_shader_pool( );

	typedef pool<sizeof(effect), 1024>		effect_allocator;

	effect_allocator&	get_effect_allocator( );
}

} // namespace render

#endif // #ifndef __render_render_resources_h_included_