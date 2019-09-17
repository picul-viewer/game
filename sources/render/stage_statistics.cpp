#include "stage_statistics.h"
#include <resource_system/api.h>
#include <resources/resources_path.h>
#include <ui/font_cook.h>

#include "resources.h"
#include "parameters.h"
#include "statistics.h"

namespace render {

void stage_statistics::create( )
{
	g_statistics.create( );

	ui::font_cook* const font_cook = ui::font_cook::create( GET_RESOURCE_PATH( "configs\\fonts\\console.cfg" ) );
	resource_system::create_resources(
		resource_system::user_callback_task<stage_statistics, &stage_statistics::on_font_loaded>( this ),
		font_cook
	);
}

void stage_statistics::on_font_loaded( queried_resources& in_resources )
{
	m_font = in_resources.get_resource<ui::font::ptr>( );
}

void stage_statistics::destroy( )
{
	g_statistics.destroy( );

	m_font->release( );
	m_font.reset( );
}

void stage_statistics::execute( )
{
	uptr const frame_delay = 7;

	g_statistics.process_frame( frame_delay );

	if ( ( m_font != nullptr ) && ( g_parameters.draw_statistics ) )
	{
		str256 statistics_string;
		g_statistics.output_render_statistics( frame_delay, statistics_string.data( ), str256::max_string_size - 1 );

		ui_batch& batch = g_resources.get_ui_batch( );

		m_font->render_multiline_string(
			statistics_string.c_str( ), statistics_string.length( ),
			math::u16x2( 0, 0 ), 0, 0, math::half4( 1.0f, 1.0f, 0.0f, 1.0f ),
			batch );
		batch.next_level( );
	}
}

} // namespace render