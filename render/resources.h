#ifndef __render_resources_h_included_
#define __render_resources_h_included_

#include <types.h>

#include "resource_views.h"
#include "render_targets.h"

#include "texture.h"
#include "resource_pool.h"
#include "pipeline_state_pool.h"

#include "sampler.h"
#include "mesh.h"
#include "constant_buffer.h"

#include "render_object_allocator.h"

#include "shader_containers.h"

#include "render_model_mesh.h"

#include "scene.h"
#include "object.h"

namespace render {

class resources
{
public:
	typedef resource_pool<mesh>		mesh_pool;
	typedef resource_pool<texture>	texture_pool;

	typedef resource_pool<render_model_mesh>	render_model_mesh_pool;

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

	typedef pool<sizeof(scene), Memory_Page_Size> scene_pool;
	
	typedef dynamic_pool<sizeof(object), Memory_Page_Size, 256> object_pool;

public:
	void create( );
	void destroy( );
	
	inline render_target_view& get_backbuffer( ) { return m_backbuffer; }
	inline render_target_tex2d& get_depth_buffer( ) { return m_depth_buffer; }

	inline mesh_pool& get_mesh_pool( ) { return m_mesh_pool; }
	inline texture_pool& get_texture_pool( ) { return m_texture_pool; }

	inline pipeline_state_pool& get_pipeline_state_pool( ) { return m_pipeline_state_pool; }

	sampler& get_default_sampler( u32 in_index );
	mesh& get_default_mesh( u32 in_index );
	constant_buffer& get_default_constant_buffer( u32 in_index );

	inline render_object_allocator& get_render_object_allocator( ) { return m_render_object_allocator; }
	
	inline vertex_shader_container&		get_vertex_shader_container( ) { return m_vertex_shader_container; }
	inline pixel_shader_container&		get_pixel_shader_container( ) { return m_pixel_shader_container; }
	inline geometry_shader_container&	get_geometry_shader_container( ) { return m_geometry_shader_container; }
	inline hull_shader_container&		get_hull_shader_container( ) { return m_hull_shader_container; }
	inline domain_shader_container&		get_domain_shader_container( ) { return m_domain_shader_container; }
	inline compute_shader_container&	get_compute_shader_container( ) { return m_compute_shader_container; }
	
	inline render_model_mesh_pool& get_render_model_mesh_pool( ) { return m_render_model_mesh_pool; }

	inline scene_pool& get_scene_pool( ) { return m_scene_pool; }

	inline object_pool& get_object_pool( ) { return m_object_pool; }

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
	render_target_view			m_backbuffer;
	render_target_tex2d			m_depth_buffer;

	mesh_pool					m_mesh_pool;
	texture_pool				m_texture_pool;
	pipeline_state_pool			m_pipeline_state_pool;

	sampler						m_default_samplers[default_sampler_type_count];
	mesh						m_default_meshes[default_mesh_type_count];
	constant_buffer				m_default_constant_buffers[default_constant_buffer_type_count];

	render_object_allocator		m_render_object_allocator;

	vertex_shader_container		m_vertex_shader_container;
	pixel_shader_container		m_pixel_shader_container;
	geometry_shader_container	m_geometry_shader_container;
	hull_shader_container		m_hull_shader_container;
	domain_shader_container		m_domain_shader_container;
	compute_shader_container	m_compute_shader_container;

	render_model_mesh_pool		m_render_model_mesh_pool;

	scene_pool					m_scene_pool;
	object_pool					m_object_pool;
};


extern resources g_resources;

} // namespace render

#endif // #ifndef __render_resources_h_included_