#include "fly_camera.h"
#include <system/timer.h>
#include <math/math_3d.h>

namespace game {

void fly_camera::create( math::float3 position, math::float2 rotation, math::float3 speed_values, float mouse_sensitivity, sys::window_input_state* input_state )
{
	m_input_state = input_state;
	m_position = position;
	m_rotation = rotation;
	m_speed_values = speed_values;
	m_mouse_sensitivity = mouse_sensitivity;
}

void fly_camera::update( float delta_time )
{
	m_rotation = - (math::float2)m_input_state->get_mouse( ).mouse_position * m_mouse_sensitivity;

	float const speed_value = m_input_state->get_keyboard( ).is_key_pressed( sys::key::shift ) ? m_speed_values.z :
							( m_input_state->get_keyboard( ).is_key_pressed( sys::key::control ) ? m_speed_values.x : m_speed_values.y );

	math::float2 xz_step( 0.0f, 0.0f );

	if ( m_input_state->get_keyboard( ).is_key_pressed( sys::key::letter_w ) )
		xz_step.y += delta_time * speed_value;

	if ( m_input_state->get_keyboard( ).is_key_pressed( sys::key::letter_s ) )
		xz_step.y -= delta_time * speed_value;

	if ( m_input_state->get_keyboard( ).is_key_pressed( sys::key::letter_a ) )
		xz_step.x -= delta_time * speed_value;

	if ( m_input_state->get_keyboard( ).is_key_pressed( sys::key::letter_d ) )
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