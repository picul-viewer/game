#ifndef __engine_object_h_included_
#define __engine_object_h_included_

#include <types.h>
#include <math/matrix.h>
#include <lib/reader.h>

namespace render { class object; }

namespace engine {

class object
{
public:
	void create( lib::reader& in_reader );
	void destroy( );

	void update( math::float4x3 const& in_transform );

	inline render::object* get_render( ) { return m_render; }

protected:
	// Aligned by 64 bytes
	math::float4x3			m_transform;

	render::object*			m_render;

	//physics::game_object	m_physic;
	// Or something to handle physics...
	
	// Something to handle sound...

public:
	object*					m_next;
};

} // namespace engine

#endif // #ifndef __engine_object_h_included_