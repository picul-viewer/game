#ifndef __render_render_api_h_included_
#define __render_render_api_h_included_

#include <types.h>
#include <core/math.h>
#include "dx_include.h"

namespace render {

namespace api
{
	static const DXGI_FORMAT backbuffer_pixel_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	void create	( );// HWND in_hwnd, math::u16x2 in_resolution, DXGI_FORMAT in_pixel_format, bool in_is_windowed, bool in_allow_debug );
	void destroy( );

	ID3D11Device*			get_device			( );
	IDXGISwapChain*			get_swap_chain		( );
	ID3D11DeviceContext*	get_context			( );
	ID3D11Texture2D*		get_backbuffer		( );
	D3D_FEATURE_LEVEL		get_feature_level	( );
	
} // namespace render_api

} // namespace render

#endif // #ifndef __render_render_api_h_included_