#ifndef __demo_fly_camera_h_included_
#define __demo_fly_camera_h_included_

#include <types.h>
#include <math/vector.h>
#include <math/matrix.h>

namespace game {

class fly_camera
{
public:
	void create( math::float3 position, math::float2 rotation, math::float3 speed_values, float mouse_sensitivity );

	void update( float delta_time );
	math::float4x3 get_view_matrix( ) const;

protected:
	math::float3 m_position;
	math::float2 m_rotation;
	math::float3 m_speed_values;
	float m_mouse_sensitivity;

};

} // namespace game

#endif // #ifndef __demo_fly_camera_h_included_