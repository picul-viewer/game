#include "stage_statistics.h"
#include <lib/allocator.h>
#include <system/file.h>
#include <utils/resources_path.h>
#include "resources.h"
#include "parameters.h"
#include "statistics.h"

namespace render {

void stage_statistics::create( )
{
	pcstr const font_path = GET_RESOURCE_PATH( "configs\\fonts\\console.cfg" );

	sys::file f( font_path, sys::file::open_read );
	ASSERT( f.is_valid( ) );
	uptr const size = f.size( );
	pvoid const data = stack_allocate( size );
	f.read( data, size );
	f.close( );

	m_font.create( reader( data, size ) );

	g_statistics.create( );
}

void stage_statistics::destroy( )
{
	m_font.destroy( );
	
	g_statistics.destroy( );
}

void stage_statistics::execute( )
{
	uptr const frame_delay = 7;

	g_statistics.process_frame( frame_delay );

	if ( g_parameters.draw_statistics )
	{
		str256 statistics_string;
		g_statistics.output_render_statistics( frame_delay, statistics_string.data( ), str256::max_string_size - 1 );

		ui_batch& batch = g_resources.get_ui_batch( );

		m_font.render_multiline_string(
			statistics_string.c_str( ), statistics_string.length( ),
			math::u16x2( 0, 0 ), 0, 0, math::half4( 1.0f, 1.0f, 0.0f, 1.0f ),
			batch );
		batch.next_level( );
	}
}

} // namespace render