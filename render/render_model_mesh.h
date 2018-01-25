#ifndef __render_render_model_mesh_h_included_
#define __render_render_model_mesh_h_included_


#include <core/types.h>
#include <core/math.h>
#include <core/game.h>
#include "mesh.h"
#include "texture.h"

namespace render {

class render_model_mesh
{
public:
	void create( config& in_config );
	void destroy( );
	
	void render( ) const;

	inline aabb_aligned const& get_aabb( ) const { return m_aabb; }

protected:
	aabb_aligned		m_aabb;

	mesh*				m_mesh;

	texture*			m_diffuse;
	texture*			m_specular;
};

} // namespace render

#endif // #ifndef __render_render_model_mesh_h_included_