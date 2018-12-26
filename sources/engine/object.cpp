#include "object.h"

#include <render/object.h>
#include <render/world.h>

namespace engine {

void object::create( reader& in_reader )
{
	ASSERT( in_reader.is_valid( ) );

	bool const create_render = in_reader.read<bool>( );
		
	if ( create_render )
	{
		m_render = render::g_world.create_object( );
		m_render->create( in_reader );
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