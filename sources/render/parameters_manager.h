#ifndef __render_parameters_manager_h_included_
#define __render_parameters_manager_h_included_

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

#endif // #ifndef __render_parameters_manager_h_included_