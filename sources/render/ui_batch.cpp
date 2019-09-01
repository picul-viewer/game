#include "ui_batch.h"
#include <lib/algorithms.h>
#include "parameters.h"
#include "texture.h"

namespace render {

void ui_batch::create( )
{
	m_temporal_memory.create( nullptr, max_vertices );
	m_buffer.create( nullptr, max_vertices );
	m_batch_data.create( nullptr, max_batches );
}

void ui_batch::destroy( )
{
	m_temporal_memory.destroy( );
	m_buffer.destroy( );
	m_batch_data.destroy( );
}

void ui_batch::add_quad(
	math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
	math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_handle const in_texture )
{
	ui_quad_data& data = m_temporal_memory.emplace_back( );

	data.corners_position	= in_corners_position;
	data.corners_texcoord	= in_corners_texcoord;
	data.mult_color			= in_mult_color;
	data.add_color			= in_add_color;
	data.texture			= in_texture;
}

void ui_batch::add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color )
{
	ui_quad_data& data = m_temporal_memory.emplace_back( );

	data.corners_position	= in_corners_position;
	data.corners_texcoord	= math::u16x4( 0, 0, 0, 0 );
	data.mult_color			= math::half4( 0.0f, 0.0f, 0.0f, 0.0f );
	data.add_color			= in_color;
	data.texture			= texture_handle::invalid;
}

void ui_batch::next_level( )
{
	ui_quad_data* const begin = m_temporal_memory.begin( );
	ui_quad_data* const end = m_temporal_memory.end( );

	ASSERT_CMP( m_temporal_memory.size( ), <, 4 * Gb );
	u32 const count = (u32)m_temporal_memory.size( );

	if ( count == 0 )
		return;

	auto const comparator = []( ui_quad_data const& l, ui_quad_data const& r )
	{
		return l.texture < r.texture;
	};

	lib::sort( begin, end, comparator );
	
	math::u32x4 const half_dimensions = math::u16x4( g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, g_parameters.screen_resolution.x, g_parameters.screen_resolution.y );

	ui_quad_data* i = m_temporal_memory.data( );

	ASSERT_CMP( m_buffer.size( ), <, 4 * Gb );
	u32 const size = (u32)m_buffer.size( );
	gpu_vertex_data* o = m_buffer.end( );
	m_buffer.reserve( size + count * 4 );

	batch_data* data = &m_batch_data.emplace_back( );
	data->vertex_start_index = size;
	data->texture = i->texture;
	
	for ( u32 index = 0; index < count; ++i, ++index )
	{
		if ( i->texture != data->texture )
		{
			data->vertex_count = size + index - data->vertex_start_index;

			data = &m_batch_data.emplace_back( );
			data->texture = i->texture;
			data->vertex_start_index = size + index;
		}

		math::s16x4 normalized_position = math::u16x4( math::u32x4( i->corners_position ) * math::u32x4( 65535 ) / half_dimensions ) - math::u16x4( 32768 );
		normalized_position.y = 65535 - normalized_position.y;
		normalized_position.w = 65535 - normalized_position.w;

		o->position = math::s16x2( normalized_position.x, normalized_position.y );
		o->texcoord = math::u16x2( i->corners_texcoord.x, i->corners_texcoord.y );
		o->mult_color = i->mult_color;
		o->add_color = i->add_color;
		++o;
		
		o->position = math::s16x2( normalized_position.z, normalized_position.y );
		o->texcoord = math::u16x2( i->corners_texcoord.z, i->corners_texcoord.y );
		o->mult_color = i->mult_color;
		o->add_color = i->add_color;
		++o;
		
		o->position = math::s16x2( normalized_position.x, normalized_position.w );
		o->texcoord = math::u16x2( i->corners_texcoord.x, i->corners_texcoord.w );
		o->mult_color = i->mult_color;
		o->add_color = i->add_color;
		++o;
		
		o->position = math::s16x2( normalized_position.z, normalized_position.w );
		o->texcoord = math::u16x2( i->corners_texcoord.z, i->corners_texcoord.w );
		o->mult_color = i->mult_color;
		o->add_color = i->add_color;
		++o;
	}

	data->vertex_count = size + count * 4 - data->vertex_start_index;

	m_temporal_memory.clear( );
}

void ui_batch::clear( )
{
	m_temporal_memory.clear( );
	m_buffer.clear( );
	m_batch_data.clear( );
}

} // namespace render