#ifndef GUARD_RENDER_PARAMETERS_MANAGER_H_INCLUDED
#define GUARD_RENDER_PARAMETERS_MANAGER_H_INCLUDED

#include <types.h>
#include "parameters.h"

namespace render {

class parameters_manager
{
public:
	parameters& get_parameters_for_modification( );

	void update( );

private:
	parameters m_parameters;

};

extern parameters_manager g_parameters_manager;

} // namespace render

#endif // #ifndef GUARD_RENDER_PARAMETERS_MANAGER_H_INCLUDED