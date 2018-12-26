#ifndef __render_render_object_mesh_h_included_
#define __render_render_object_mesh_h_included_

#include <types.h>

#include <math/math_sse.h>
#include <math/aabb.h>

#include <lib/reader.h>

#include "render_object.h"
#include "render_model_mesh.h"

namespace render {

class render_object_mesh : public render_object
{
public:
	void create( reader& in_reader );
	void destroy( );

	void update( math::float4x3 const& in_transform );

	void render( ) const;

	inline render_model_mesh_handle get_render_model( ) const { return m_render_model; }

	inline math::sse::matrix3 const& get_transform( ) const { return m_transform; }
	inline math::aabb_aligned const& get_aabb( ) const { return m_aabb; }

protected:
	render_model_mesh_handle	m_render_model;
	u16							m_padding[3];

	// This is 16-byte aligned
	math::sse::matrix3			m_local_transform;
	math::sse::matrix3			m_transform;
	math::aabb_aligned			m_aabb;
};

} // namespace render

#endif // #ifndef __render_render_object_mesh_h_included_