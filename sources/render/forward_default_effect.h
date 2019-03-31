#ifndef GUARD_RENDER_FORWARD_DEFAULT_EFFECT_H_INCLUDED
#define GUARD_RENDER_FORWARD_DEFAULT_EFFECT_H_INCLUDED

#include "effect.h"
#include <core/game.h>
#include "miscellaneous_constant_buffer.h"
#include "shader_pool.h"

namespace render {

class forward_default_effect : public effect
{
public:
	void create( config in_config ) override;

	virtual void apply( u8					in_stencil_ref	= 0u,
						math::float4 const&	in_blend_factor	= math::float4( 0.0f, 0.0f, 0.0f, 0.0f ),
						u32					in_sample_mask	= 0xffffffff ) const override;
	
	virtual void destroy( ) override;

protected:
	enum config_value
	{
		diffuse_texture		= 0,
		diffuse_value		= 1,

		specular_texture	= 2,
		specular_value		= 3,

		smoothness_texture	= 4,
		smoothness_value	= 5,

		metalness_texture	= 6,
		metalness_value		= 7
	};

	struct miscellaneous_constants
	{
		math::float3	m_diffuse;
		float			m_specular;
		float			m_smoothness;
		float			m_metalness;
	};
	
	enum
	{
		pixel_shader_macro_use_diffuse_texture = 0,
		pixel_shader_macro_use_specular_texture,
		pixel_shader_macro_use_smoothness_texture,
		pixel_shader_macro_use_metalness_texture
	};

	texture*				m_diffuse_texture;
	texture*				m_specular_texture;
	texture*				m_smoothness_texture;
	texture*				m_metalness_texture;

	miscellaneous_constants	m_constants;

	bool					m_exist_texture;
	bool					m_exist_value;
};

} // namespace redner

#endif // #ifndef GUARD_RENDER_FORWARD_DEFAULT_EFFECT_H_INCLUDED