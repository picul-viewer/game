#ifndef __render_render_parameters_h_included_
#define __render_render_parameters_h_included_

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
	bool			is_d3d_debug;
};

extern parameters g_parameters;

} // namespace render

#endif // #ifndef __render_render_parameters_h_included_