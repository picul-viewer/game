#ifndef __render_render_object_mesh_h_included_
#define __render_render_object_mesh_h_included_

#include <types.h>

#include <math/math_sse.h>
#include <math/aabb.h>

#include <lib/binary_config.h>

#include "render_object.h"

#include "render_model_mesh.h"

namespace render {

class render_object_mesh : public render_object
{
public:
	void create( binary_config& in_config );
	void destroy( );

	void update( math::sse::matrix3 in_transform );

	void render( ) const;

	inline render_model_mesh* get_render_model( ) const { return m_render_model; }

	inline math::sse::matrix3 const& get_transform( ) const { return m_transform; }
	inline aabb_aligned const& get_aabb( ) const { return m_aabb; }

protected:
	render_model_mesh*	m_render_model;

	// This is 16-byte aligned
	math::sse::matrix3	m_local_transform;
	math::sse::matrix3	m_transform;
	aabb_aligned		m_aabb;
};

} // namespace render

#endif // #ifndef __render_render_object_mesh_h_included_