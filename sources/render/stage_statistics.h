#ifndef __render_stage_statistics_h_included_
#define __render_stage_statistics_h_included_

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

#endif // #ifndef __render_stage_statistics_h_included_