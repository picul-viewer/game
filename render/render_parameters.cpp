#include "render_parameters.h"

namespace render {

namespace parameters
{

struct data
{
	HWND			m_hwnd;
	math::u16x2		m_resolution;
	bool			m_is_windowed;
	bool			m_allow_debug;
};

data m_current;
data m_next;
bool m_updated;

// Setters
void set_hwnd( HWND in_hwnd )
{
	m_next.m_hwnd		= in_hwnd;
	m_updated			= true;
}

void set_resolution( math::u16x2 in_resolution )
{
	m_next.m_resolution = in_resolution;
	m_updated			= true;
}

void set_windowed( bool in_is_windowed )
{
	m_next.m_is_windowed	= in_is_windowed;
	m_updated				= true;
}

void set_debug( bool in_allow_debug )
{
	m_next.m_allow_debug	= in_allow_debug;
	m_updated				= true;
}

// Getters
HWND get_hwnd( )
{
	return m_current.m_hwnd;
}

math::u16x2 get_resolution( )
{
	return m_current.m_resolution;
}

bool get_windowed( )
{
	return m_current.m_is_windowed;
}

bool get_debug( )
{
	return m_current.m_allow_debug;
}


void update( )
{
	if ( m_updated )
	{
		m_current = m_next;
		m_updated = false;
	}
}

} // namespace render_parameters

} // namespace render