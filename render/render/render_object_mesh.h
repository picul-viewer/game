#ifndef __render_render_object_mesh_h_included_
#define __render_render_object_mesh_h_included_

#include <core/types.h>
#include <core/math.h>
#include <core/game.h>
#include "mesh.h"
#include "forward_default_effect.h"
#include "resource_views.h"

namespace render {

class render_object_mesh : public render_object
{
public:
	render_object_mesh( );

	void create( config& in_config );
	virtual void destroy( ) override;

	virtual void dispatch( ) const override;

	void render( ) const;

	//inline void set_skeleton( skeleton* in_skeleton ) { m_skeleton = in_skeleton; }
	//inline skeleton* get_skeleton( ) { return m_skeleton; }

protected:
	mesh*					m_mesh;
	forward_default_effect*	m_effect;
	//skeleton*				m_skeleton;
};

class render_object_mesh_transformed : public render_object_mesh
{
public:
	render_object_mesh_transformed( );

	void create( config& in_config );
	
	virtual void update_transform( ) override;

protected:
	math::float4x4	m_identity_transform;
};

} // namespace render

#endif // #ifndef __render_render_object_mesh_h_included_