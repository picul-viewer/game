#include "render_resources.h"

namespace render {

namespace resources
{

mesh_pool			m_mesh_pool;
texture_pool		m_texture_pool;
pipeline_pool		m_pipeline_pool;
shader_pool			m_shader_pool;

effect_allocator	m_effect_allocator;

static pcstr c_mesh_path	= "resources/meshes/";
static pcstr c_texture_path	= "resources/textures/";

void create( )
{
	m_mesh_pool.set_resource_path( c_mesh_path );
	m_texture_pool.set_resource_path( c_texture_path );
}

void destroy( )
{
	m_pipeline_pool.destroy( );
}

mesh_pool& get_mesh_pool( )
{
	return m_mesh_pool;
}

texture_pool& get_texture_pool( )
{
	return m_texture_pool;
}

pipeline_pool& get_pipeline_pool( )
{
	return m_pipeline_pool;
}

shader_pool& get_shader_pool( )
{
	return m_shader_pool;
}

effect_allocator& get_effect_allocator( )
{
	return m_effect_allocator;
}

}

} // namespace render