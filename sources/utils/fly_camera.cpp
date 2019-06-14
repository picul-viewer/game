#include "fly_camera.h"
#include <math/math_3d.h>
#include <system/window_input_state.h>

namespace utils {

void fly_camera::create( math::float3 const& position, math::float2 const& rotation, math::float3 const& speed_values, float const mouse_sensitivity )
{
	m_position = position;
	m_rotation = rotation;
	m_speed_values = speed_values;
	m_last_mouse_position = sys::g_input_state.get_mouse( ).mouse_position;
	m_mouse_sensitivity = mouse_sensitivity;
	m_enabled = true;
}

void fly_camera::enable( bool const enabled )
{
	m_enabled = enabled;
}

void fly_camera::update( float delta_time )
{
	using namespace sys;

	if ( !m_enabled )
	{
		m_last_mouse_position = g_input_state.get_mouse( ).mouse_position;
		return;
	}

	math::s32x2 const& current_position = g_input_state.get_mouse( ).mouse_position;
	m_rotation -= math::float2( current_position - m_last_mouse_position ) * m_mouse_sensitivity;
	m_last_mouse_position = current_position;

	float const speed_value = g_input_state.get_keyboard( ).is_key_pressed( sys::key::shift ) ? m_speed_values.z :
							( g_input_state.get_keyboard( ).is_key_pressed( sys::key::control ) ? m_speed_values.x : m_speed_values.y );

	math::float2 xz_step( 0.0f, 0.0f );

	if ( g_input_state.get_keyboard( ).is_key_pressed( sys::key::letter_w ) )
		xz_step.y += delta_time * speed_value;

	if ( g_input_state.get_keyboard( ).is_key_pressed( sys::key::letter_s ) )
		xz_step.y -= delta_time * speed_value;

	if ( g_input_state.get_keyboard( ).is_key_pressed( sys::key::letter_a ) )
		xz_step.x -= delta_time * speed_value;

	if ( g_input_state.get_keyboard( ).is_key_pressed( sys::key::letter_d ) )
		xz_step.x += delta_time * speed_value;

	math::float3 z_direction;
	z_direction.x = math::cos( m_rotation.y ) * math::cos( m_rotation.x );
	z_direction.y = math::sin( m_rotation.y );
	z_direction.z = math::cos( m_rotation.y ) * math::sin( m_rotation.x );

	math::float3 const x_direction = math::cross( math::float3( 0.0f, 1.0f, 0.0f ), z_direction );

	m_position += xz_step.x * x_direction + xz_step.y * z_direction;
}

math::float4x3 fly_camera::get_view_matrix( ) const
{
	math::float3 direction;
	direction.x = math::cos( m_rotation.y ) * math::cos( m_rotation.x );
	direction.y = math::sin( m_rotation.y );
	direction.z = math::cos( m_rotation.y ) * math::sin( m_rotation.x );

	return math::matrix_look_at_direction(
		m_position,
		direction,
		math::float3( 0.0f, 1.0f, 0.0f )
	);
}

} // namspace game