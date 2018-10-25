#include "parameters_manager.h"

namespace render {

parameters& parameters_manager::get_parameters_for_modification( )
{
	return m_parameters;
}

void parameters_manager::update( )
{
	g_parameters = m_parameters;
}

parameters_manager g_parameters_manager;

} // namespace render