#ifndef GUARD_RENDER_STAGE_STATISTICS_H_INCLUDED
#define GUARD_RENDER_STAGE_STATISTICS_H_INCLUDED

#include <ui/font.h>

namespace render {

class stage_statistics
{
public:
	void create( );
	void destroy( );

	void execute( );

protected:
	ui::font m_font;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_STAGE_STATISTICS_H_INCLUDED