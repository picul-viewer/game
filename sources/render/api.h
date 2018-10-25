#ifndef __render_api_h_included_
#define __render_api_h_included_

#include <types.h>
#include "dx_include.h"

namespace render {

class api
{
public:
	static const DXGI_FORMAT backbuffer_pixel_format;

	void create	( );
	void destroy( );

	ID3D11Device*			get_device			( );
	IDXGISwapChain*			get_swap_chain		( );
	ID3D11DeviceContext*	get_context			( );
	ID3D11Texture2D*		get_backbuffer		( );
	D3D_FEATURE_LEVEL		get_feature_level	( );
	
protected:
	ID3D11Device*			m_device;
	IDXGISwapChain*			m_swap_chain;
	ID3D11DeviceContext*	m_context;
	ID3D11Texture2D*		m_backbuffer;
	D3D_FEATURE_LEVEL		m_feature_level;

};


extern api g_api;

} // namespace render

#endif // #ifndef __render_api_h_included_