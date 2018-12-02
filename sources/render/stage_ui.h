#ifndef __render_stage_ui_h_included_
#define __render_stage_ui_h_included_

#include "effect.h"

namespace render {

class stage_ui
{
public:
	void create( );
	void destroy( );

	void execute( );

protected:
	effect<1> m_ui_default_effect;

};

} // namespace render

#endif // #ifndef __render_stage_ui_h_included_