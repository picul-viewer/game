#ifndef GUARD_RENDER_STAGE_UI_H_INCLUDED
#define GUARD_RENDER_STAGE_UI_H_INCLUDED

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

#endif // #ifndef GUARD_RENDER_STAGE_UI_H_INCLUDED