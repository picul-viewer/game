#include "render_object_mesh.h"
#include "render_resources.h"
#include "renderer.h"

namespace render {

render_object_mesh::render_object_mesh( ) :
	render_object( render_object_type::render_object_type_mesh )
{ }

void render_object_mesh::create( config& in_config )
{
	pcstr mesh_path	= in_config.read<pcstr>( );
	m_mesh			= resources::get_mesh_pool( ).create_resource( mesh_path );

	m_effect		= (forward_default_effect*)resources::get_effect_allocator( ).allocate( );
	m_effect->create( in_config );
}

void render_object_mesh::destroy( )
{
	resources::get_mesh_pool( ).free_resource( m_mesh );
}

void render_object_mesh::dispatch( ) const
{
	renderer::dispatch_object_mesh( this );
}

void render_object_mesh::render( ) const
{
	m_effect->apply	( );
	m_mesh->draw	( );
}


render_object_mesh_transformed::render_object_mesh_transformed( ) :
	render_object_mesh( )
{ }

void render_object_mesh_transformed::create( config& in_config )
{
	m_transform	= in_config.read<math::float4x4>( );
	render_object_mesh::create( in_config );
}

void render_object_mesh_transformed::update_transform( )
{
	m_transform = math::combine_transforms( m_identity_transform, m_parent->get_transform( ) );
}

} // namespace render