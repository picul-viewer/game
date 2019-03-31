#ifndef GUARD_RENDER_PARAMETERS_TYPE_H_INCLUDED
#define GUARD_RENDER_PARAMETERS_TYPE_H_INCLUDED

#include <types.h>
#include <math/vector.h>
#include <math/matrix.h>

namespace render {

struct parameters
{
	struct
	{
		math::float4x4 view;
		float fov;
	} camera;

	pvoid			hwnd;
	math::u16x2		screen_resolution;
	bool			is_windowed;
	bool			draw_statistics;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_PARAMETERS_TYPE_H_INCLUDED