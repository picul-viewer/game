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
	void create( binary_config& in_config );

	u32 add_ref( );
	u32 release( );

	void set_registry_pointer( pointer in_pointer );
	pointer get_registry_pointer( ) const;

	void render( ) const;
	
	inline math::aabb_aligned& get_aabb( ) { return m_aabb; }
	inline math::aabb_aligned const& get_aabb( ) const { return m_aabb; }
	
	inline mesh* get_mesh( ) const { return m_mesh; }

	inline texture* get_diffuse_texture( ) const { return m_diffuse; }
	inline texture* get_specular_texture( ) const { return m_specular; }
	
protected:
	// Use unused w-coordinate of AABB min as reference counter.
	union
	{
		math::aabb_aligned	m_aabb;
		struct
		{
			u32 unused0[3];
			u32 m_ref_counter;
			u32 unused1[4];
		};
	};

	mesh*				m_mesh;

	texture*			m_diffuse;
	texture*			m_specular;

	pointer				m_registry_pointer;
};

} // namespace render

#endif // #ifndef __render_render_model_mesh_h_included_