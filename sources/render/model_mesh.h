#ifndef GUARD_RENDER_MODEL_MESH_H_INCLUDED
#define GUARD_RENDER_MODEL_MESH_H_INCLUDED

#include <types.h>
#include <math/aabb.h>

#include <resource_system/shared_resource.h>
#include "model_mesh_cook.h"
#include "handles.h"

#include "gpu_structures.h"
#include "mesh.h"
#include "texture.h"

namespace render {

class model_mesh : public shared_resource<model_mesh, model_mesh_handle, 4096>
{
public:
	friend class model_mesh_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_main
	};

	static void destroy_resource( model_mesh* const in_resource );

public:
	inline math::aabb_aligned& get_aabb( ) { return m_aabb; }
	inline math::aabb_aligned const& get_aabb( ) const { return m_aabb; }

	inline mesh* get_mesh( ) const { return mesh::from_handle( m_mesh ); }

	inline texture* get_albedo_texture( ) const { return texture::from_handle( m_albedo ); }
	inline texture* get_metal_rough_texture( ) const { return texture::from_handle( m_metal_rough ); }

private:
	friend class render_object_mesh;

	void fill_gpu_data( gpu::mesh_object& in_data );

private:
	math::aabb_aligned	m_aabb;

	mesh_handle			m_mesh;

	texture_handle		m_albedo;
	texture_handle		m_metal_rough;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_MODEL_MESH_H_INCLUDED