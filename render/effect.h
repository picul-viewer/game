#ifndef __render_effect_h_included_
#define __render_effect_h_included_

#include <types.h>
#include <math/vector.h>
#include "dx_include.h"

#include "pipeline_state.h"
#include "shader_containers.h"

namespace render {

class technique
{
public:
	void create( );
	void destroy( );
	
	void set_depth_stencil_state( depth_stencil_state::cook const& in_cook );
	void set_blend_state		( blend_state::cook const& in_cook );
	void set_rasterizer_state	( rasterizer_state::cook const& in_cook );
	
	void set_vertex_shader		( vertex_shader_type in_shader_type );
	void set_pixel_shader		( pixel_shader_type in_shader_type );
	void set_geometry_shader	( geometry_shader_type in_shader_type );
	
	inline void set_stencil_ref	( u8 in_stencil_ref );
	inline void set_blend_factor( math::float4 in_blend_factor );
	inline void set_sample_mask	( u32 in_sample_mask );

	inline u8			get_stencil_ref	(  ) const;
	inline math::float4	get_blend_factor(  ) const;
	inline u32			get_sample_mask	(  ) const;

	void apply( ) const;
	void apply( u8 in_stencil_ref,
				math::float4 in_blend_factor,
				u32 in_sample_mask ) const;

protected:
	depth_stencil_state	m_depth_stencil_state;
	blend_state			m_blend_state;
	rasterizer_state	m_rasterizer_state;

	vertex_shader		m_vertex_shader;
	pixel_shader		m_pixel_shader;
	geometry_shader		m_geometry_shader;

	math::float4		m_blend_factor;
	u32					m_sample_mask;
	u8					m_stencil_ref;
};

template<uptr TechniqueCount>
class effect
{
public:
	void create( );
	void destroy( );

	technique& operator[]( uptr in_index );
	technique const& operator[]( uptr in_index ) const;

	void apply( uptr in_index = 0 ) const;

protected:
	technique m_techniques[TechniqueCount];
};

} // namespace render

#include "effect_inline.h"

#endif // #ifndef __render_effect_h_included_