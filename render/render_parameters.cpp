#include "render_parameters.h"

namespace render {

parameters::data const* parameters::operator->( )
{
	return &m_current;
}

parameters::data const* parameters::current( ) const
{
	return &m_current;
}

parameters::data* parameters::next( )
{
	return &m_next;
}

void parameters::update( )
{
	m_current = m_next;
}


parameters g_parameters;

} // namespace render