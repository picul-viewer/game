#ifndef GUARD_RENDER_STAGE_FORWARD_DEFAULT_H_INCLUDED
#define GUARD_RENDER_STAGE_FORWARD_DEFAULT_H_INCLUDED

#include "effect.h"

namespace render {

class stage_forward_default
{
public:
	void create( );
	void destroy( );

	void execute( );

protected:
	effect<1> m_forward_default_effect;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_STAGE_FORWARD_DEFAULT_H_INCLUDED