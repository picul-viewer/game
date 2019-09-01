#ifndef GUARD_RENDER_RESOURCES_H_INCLUDED
#define GUARD_RENDER_RESOURCES_H_INCLUDED

#include <types.h>

#include "resource_views.h"
#include "render_targets.h"

#include "pipeline_state_pool.h"

#include "sampler.h"
#include "constant_buffer.h"

#include "render_object_allocator.h"

#include "shader_containers.h"

#include "scene.h"
#include "object.h"

#include "ui_batch.h"

namespace render {

class resources
{
public:
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
		default_sampler_type_linear_clamp,

		default_sampler_type_count
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
	inline render_target_depth& get_depth_buffer( ) { return m_depth_buffer; }

	inline pipeline_state_pool& get_pipeline_state_pool( ) { return m_pipeline_state_pool; }

	sampler& get_default_sampler( u32 in_index );
	constant_buffer& get_default_constant_buffer( u32 in_index );

	inline render_object_allocator& get_render_object_allocator( ) { return m_render_object_allocator; }
	
	inline vertex_shader_container&		get_vertex_shader_container( ) { return m_vertex_shader_container; }
	inline pixel_shader_container&		get_pixel_shader_container( ) { return m_pixel_shader_container; }
	inline geometry_shader_container&	get_geometry_shader_container( ) { return m_geometry_shader_container; }
	inline hull_shader_container&		get_hull_shader_container( ) { return m_hull_shader_container; }
	inline domain_shader_container&		get_domain_shader_container( ) { return m_domain_shader_container; }
	inline compute_shader_container&	get_compute_shader_container( ) { return m_compute_shader_container; }
	
	inline vertex_buffer& get_ui_vertex_buffer( ) { return m_ui_vertex_buffer; }
	inline index_buffer& get_ui_index_buffer( ) { return m_ui_index_buffer; }

	inline ui_batch& get_ui_batch( ) { return m_ui_batch; }

	void bind_default_samplers( ) const;
	void bind_default_constant_buffers( ) const;

	void bind_default_resources( ) const;

protected:
	void create_default_samplers( );
	void destroy_default_samplers( );

	void create_default_constant_buffers( );
	void destroy_default_constant_buffers( );

	void create_shaders( );
	void destroy_shaders( );

	void create_ui_buffers( );
	void destroy_ui_buffers( );

protected:
	render_target_view			m_backbuffer;
	render_target_depth			m_depth_buffer;
	
	pipeline_state_pool			m_pipeline_state_pool;

	sampler						m_default_samplers[default_sampler_type_count];
	constant_buffer				m_default_constant_buffers[default_constant_buffer_type_count];

	render_object_allocator		m_render_object_allocator;

	vertex_shader_container		m_vertex_shader_container;
	pixel_shader_container		m_pixel_shader_container;
	geometry_shader_container	m_geometry_shader_container;
	hull_shader_container		m_hull_shader_container;
	domain_shader_container		m_domain_shader_container;
	compute_shader_container	m_compute_shader_container;

	vertex_buffer				m_ui_vertex_buffer;
	index_buffer				m_ui_index_buffer;

	ui_batch					m_ui_batch;
};


extern resources g_resources;

} // namespace render

#endif // #ifndef GUARD_RENDER_RESOURCES_H_INCLUDED