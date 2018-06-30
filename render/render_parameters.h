#ifndef __render_render_parameters_h_included_
#define __render_render_parameters_h_included_

#include <types.h>
#include "dx_include.h"

#include <math/vector.h>

namespace render {

struct parameters
{
	HWND			hwnd;
	math::u16x2		screen_resolution;
	bool			is_windowed;
	bool			is_d3d_debug;
};

extern parameters g_parameters;

} // namespace render

#endif // #ifndef __render_render_parameters_h_included_