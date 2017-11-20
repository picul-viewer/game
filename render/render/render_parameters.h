#ifndef __render_render_parameters_h_included_
#define __render_render_parameters_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"

namespace render {

namespace parameters
{
	// Setters
	void set_hwnd		( HWND in_hwnd );
	void set_resolution	( math::u16x2 in_resolution );
	void set_windowed	( bool in_is_windowed );
	void set_debug		( bool in_allow_debug );

	// Getters
	HWND		get_hwnd		( );
	math::u16x2	get_resolution	( );
	bool		get_windowed	( );
	bool		get_debug		( );
	
	void update( );

} // namespace render_parameters

} // namespace render

#endif // #ifndef __render_render_parameters_h_included_