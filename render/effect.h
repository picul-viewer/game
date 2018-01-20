#ifndef __render_effect_h_included_
#define __render_effect_h_included_

#include <core/types.h>
#include <core/math.h>
#include <core/game.h>
#include "dx_include.h"

#include "pipeline_state.h"
#include "shader.h"
#include "resource_views.h"

namespace render {

class effect
{
public:
	virtual void create( config in_config ) = 0;

	virtual void apply( u8					in_stencil_ref	= 0u,
						math::float4 const&	in_blend_factor	= math::float4( 0.0f, 0.0f, 0.0f, 0.0f ),
						u32					in_sample_mask	= 0xffffffff ) const;

	virtual void destroy( );

protected:
	depth_stencil_state*	m_depth_stencil_state;
	blend_state*			m_blend_state;
	rasterizer_state*		m_rasterizer_state;

	vertex_shader*			m_vertex_shader;
	pixel_shader*			m_pixel_shader;
	geometry_shader*		m_geometry_shader;
	
	void set_depth_stencil_state( depth_stencil_state::cook const& in_cook );
	void set_blend_state( blend_state::cook const& in_cook );
	void set_rasterizer_state( rasterizer_state::cook const& in_cook );
	
	void set_vertex_shader( pcstr in_filename, u32 in_configuration );
	void set_pixel_shader( pcstr in_filename, u32 in_configuration );
	void set_geometry_shader( pcstr in_filename, u32 in_configuration );

	inline depth_stencil_state* get_depth_stencil_state( ) const { return m_depth_stencil_state; }
	inline blend_state* get_blend_state( ) const { return m_blend_state; }
	inline rasterizer_state* get_rasterizer_state( ) const { return m_rasterizer_state; }

	inline vertex_shader* get_vertex_shader( ) const { return m_vertex_shader; }
	inline pixel_shader* get_pixel_shader( ) const { return m_pixel_shader; }
	inline geometry_shader* get_geometry_shader( ) const { return m_geometry_shader; }
};

} // namespace render

#endif // #ifndef __render_effect_h_included_