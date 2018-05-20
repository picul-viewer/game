#ifndef __render_render_resources_h_included_
#define __render_render_resources_h_included_

#include <types.h>
#include "dx_include.h"

#include "resource_views.h"
#include "render_targets.h"

#include "texture.h"
#include "resource_pool.h"
#include "pipeline_state_pool.h"
#include "shader_pool.h"

#include "sampler.h"
#include "mesh.h"
#include "constant_buffer.h"

#include "render_object_allocator.h"

#include "effect.h"

namespace render {

class resources
{
public:
	typedef resource_pool<mesh>		mesh_pool;
	typedef resource_pool<texture>	texture_pool;

	enum {
		max_depth_stencil_states	= 32,
		max_blend_states			= 16,
		max_rasterizer_states		= 16
	};

	typedef pipeline_state_pool<
		max_depth_stencil_states,
		max_blend_states,
		max_rasterizer_states
	> pipeline_state_pool;
	
	enum default_sampler_type
	{
		default_sampler_type_linear_wrap = 0,
		default_sampler_type_point_clamp,

		default_sampler_type_count
	};

	enum default_mesh_type
	{
		default_mesh_type_box = 0,
		//default_mesh_type_sphere,
		default_mesh_type_quad,
		
		default_mesh_type_count
	};

	enum default_constant_buffer_type
	{
		default_constant_buffer_type_per_frame = 0,
		default_constant_buffer_type_per_instance,
		
		default_constant_buffer_type_count
	};

public:
	void create( );
	void destroy( );
	
	inline render_target_view& get_backbuffer( ) { return m_backbuffer; }
	inline render_target_tex2d& get_depth_buffer( ) { return m_depth_buffer; }

	inline mesh_pool& get_mesh_pool( ) { return m_mesh_pool; }
	inline texture_pool& get_texture_pool( ) { return m_texture_pool; }

	inline pipeline_state_pool& get_pipeline_state_pool( ) { return m_pipeline_state_pool; }
	inline shader_pool& get_shader_pool( ) { return m_shader_pool; }

	sampler& get_default_sampler( u32 in_index );
	mesh& get_default_mesh( u32 in_index );
	constant_buffer& get_default_constant_buffer( u32 in_index );

	render_object_allocator& get_render_object_allocator( );
	
	void bind_default_samplers( ) const;
	void bind_default_constant_buffers( ) const;

	void bind_default_resources( ) const;

protected:
	void create_default_samplers( );
	void destroy_default_samplers( );

	void create_default_meshes( );
	void destroy_default_meshes( );

	void create_default_constant_buffers( );
	void destroy_default_constant_buffers( );

protected:
	render_target_view		m_backbuffer;
	render_target_tex2d		m_depth_buffer;

	mesh_pool				m_mesh_pool;
	texture_pool			m_texture_pool;
	pipeline_state_pool		m_pipeline_state_pool;
	shader_pool				m_shader_pool;

	sampler					m_default_samplers[default_sampler_type_count];
	mesh					m_default_meshes[default_mesh_type_count];
	constant_buffer			m_default_constant_buffers[default_constant_buffer_type_count];

	render_object_allocator	m_render_object_allocator;
};


extern resources g_resources;

} // namespace render

#endif // #ifndef __render_render_resources_h_included_