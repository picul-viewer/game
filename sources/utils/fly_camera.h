#ifndef __demo_fly_camera_h_included_
#define __demo_fly_camera_h_included_

#include <types.h>
#include <math/vector.h>
#include <math/matrix.h>

namespace utils {

class fly_camera
{
public:
	void create( math::float3 const& position, math::float2 const& rotation, math::float3 const& speed_values, float const mouse_sensitivity );

	void enable( bool const enabled );
	inline bool is_enabled( ) const { return m_enabled; }

	void update( float delta_time );
	math::float4x3 get_view_matrix( ) const;

protected:
	math::float3 m_position;
	math::float2 m_rotation;
	math::float3 m_speed_values;
	math::s32x2 m_last_mouse_position;
	float m_mouse_sensitivity;
	bool m_enabled;

};

} // namespace game

#endif // #ifndef __demo_fly_camera_h_included_