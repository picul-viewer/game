#ifndef __render_render_model_mesh_h_included_
#define __render_render_model_mesh_h_included_

#include <types.h>
#include <math/aabb.h>
#include <lib/binary_config.h>
#include "mesh.h"
#include "texture.h"

namespace render {

class render_model_mesh
{
public:
	render_model_mesh( );

	void create( binary_config& in_config );
	void destroy( );
	
	void render( ) const;
	
	inline aabb_aligned& get_aabb( ) { return m_aabb; }
	inline aabb_aligned const& get_aabb( ) const { return m_aabb; }
	
	inline mesh* get_mesh( ) const { return m_mesh; }

	inline texture* get_diffuse_texture( ) const { return m_diffuse; }
	inline texture* get_specular_texture( ) const { return m_specular; }
	
public:
	static void load( render_model_mesh* out_resource, weak_const_string in_filename );

protected:
	aabb_aligned		m_aabb;

	mesh*				m_mesh;

	texture*			m_diffuse;
	texture*			m_specular;
};

} // namespace render

#endif // #ifndef __render_render_model_mesh_h_included_