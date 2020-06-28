#include "camera.h"
#include <math/math_3d.h>

namespace render {

void camera::set_view( math::float4x4 const& in_view )
{
	m_view = in_view;
}

void camera::set_perspective( float in_fov, float in_aspect, float in_near, float in_far )
{
	m_projection = math::matrix_perspective( in_fov, in_aspect, in_near, in_far );
	m_near = in_near;
}

void camera::update( )
{
	// Calculate view-projection.
	m_view_projection = math::combine_transform_and_perspective_projection( m_view, m_projection );
}

void camera::fill_constant_buffer( gpu::constant_buffer& in_buffer )
{
	in_buffer.view_projection = m_view_projection;

	math::float4x4 const& inv_view = math::inverse( m_view );
	math::float4x4 const& inv_view_projection = math::inverse( m_view_projection );

	math::float4 camera_top_left_ray_world = math::modify_by_transform( math::float4( -1.0f, 1.0f, 0.0f, 1.0f ), inv_view_projection );
	camera_top_left_ray_world /= camera_top_left_ray_world.w;
	math::float4 camera_top_right_ray_world = math::modify_by_transform( math::float4( 1.0f, 1.0f, 0.0f, 1.0f ), inv_view_projection );
	camera_top_right_ray_world /= camera_top_right_ray_world.w;
	math::float4 camera_bottom_left_ray_world = math::modify_by_transform( math::float4( -1.0f, -1.0f, 0.0f, 1.0f ), inv_view_projection );
	camera_bottom_left_ray_world /= camera_bottom_left_ray_world.w;
	math::float4 camera_position_world = math::modify_by_transform( math::float4( 0.0f, 0.0f, 0.0f, 1.0f ), inv_view );
	camera_position_world /= camera_position_world.w;

	in_buffer.world_camera_position = camera_position_world;
	in_buffer.world_camera_ray_top_left = camera_top_left_ray_world - camera_position_world;
	in_buffer.world_camera_screen_ray_x = camera_top_right_ray_world - camera_top_left_ray_world;
	in_buffer.world_camera_screen_ray_y = camera_bottom_left_ray_world - camera_top_left_ray_world;

	in_buffer.deprojection_coefficients__fov_scale.x = m_projection.a22;
	in_buffer.deprojection_coefficients__fov_scale.y = m_projection.a23;

	in_buffer.deprojection_coefficients__fov_scale.z = -2.0f / m_projection.a00;
	in_buffer.deprojection_coefficients__fov_scale.w = 2.0f / m_projection.a11;
}

} // namespace render