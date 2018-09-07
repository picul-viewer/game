#include "object.h"

#include <render/object.h>
#include <render/world.h>

namespace engine {

void object::create( binary_config& in_config )
{
	ASSERT( in_config.is_valid( ) );

	bool const create_render = in_config.read<bool>( );
		
	if ( create_render )
	{
		m_render = render::g_world.create_object( );
		m_render->create( in_config );
	}
}

void object::destroy( )
{
	if ( m_render )
	{
		m_render->destroy( );
		render::g_world.destroy_object( m_render );
	}
}

void object::update( math::float4x3 const& in_transform )
{
	if ( m_render )
		m_render->update( in_transform );
}

} // namespace engine