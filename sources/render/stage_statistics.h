#ifndef GUARD_RENDER_STAGE_STATISTICS_H_INCLUDED
#define GUARD_RENDER_STAGE_STATISTICS_H_INCLUDED

#include <resource_system/queried_resources.h>
#include <ui/font.h>

namespace render {

class stage_statistics
{
public:
	void create( );
	void destroy( );

	void execute( );

	inline void set_font( ui::font::ptr const& in_font ) { m_font = in_font; }

private:
	void on_font_loaded( queried_resources& in_resources );

private:
	ui::font::ptr m_font;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_STAGE_STATISTICS_H_INCLUDED