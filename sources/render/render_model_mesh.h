#ifndef GUARD_RENDER_RENDER_MODEL_MESH_H_INCLUDED
#define GUARD_RENDER_RENDER_MODEL_MESH_H_INCLUDED

#include <types.h>
#include <math/aabb.h>
#include <lib/reader.h>
#include "mesh.h"
#include "texture.h"

namespace render {

template<typename Resource>
class resource_pool;

class render_model_mesh
{
public:
	void create( lib::reader& in_reader );

	u32 add_ref( );
	u32 release( );

	void render( ) const;
	
	inline math::aabb_aligned& get_aabb( ) { return m_aabb; }
	inline math::aabb_aligned const& get_aabb( ) const { return m_aabb; }
	
	inline mesh_handle get_mesh( ) const { return m_mesh; }

	inline texture_handle get_diffuse_texture( ) const { return m_diffuse; }
	inline texture_handle get_specular_texture( ) const { return m_specular; }

public:
	static render_model_mesh* from_handle( render_model_mesh_id const in_id );
	static render_model_mesh_id to_handle( render_model_mesh* const in_render_model_mesh );

private:
	friend class resource_pool<render_model_mesh>;

	void set_registry_pointer( pointer in_pointer );
	pointer get_registry_pointer( ) const;

private:
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

	mesh_handle			m_mesh;

	texture_handle		m_diffuse;
	texture_handle		m_specular;

	pointer				m_registry_pointer;
};

DEFINE_HANDLE( render_model_mesh );

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_MODEL_MESH_H_INCLUDED