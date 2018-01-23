#include "render_core.h"
#include <core/macros.h>
#include "render_api.h"
#include "render_resources.h"
#include "render_parameters.h"
#include "renderer.h"

namespace render {

namespace core {

thread					m_render_thread;
thread					m_context_thread;
thread					m_device_thread;

render_context_queue	m_context_queue;
render_device_queue		m_device_queue;

bool					m_keep_alive;

mt_u32					m_render_frame_id = 0;
mt_u32					m_context_frame_id = 0;

u32 render_frame_id( )
{
	return m_render_frame_id;
}

u32 context_frame_id( )
{
	return m_context_frame_id;
}

void end_frame_impl( )
{
	api::get_swap_chain( )->Present( 0, 0 );
	interlocked_inc( m_context_frame_id );

	parameters::update( );

	///
}

void end_frame( )
{
	m_context_queue.push( end_frame_impl );
}

void render_thread_func( void* )
{
	while ( m_keep_alive )
	{
		renderer::render( );
		
		m_context_queue.push( end_frame );

		// Wait while context thread is to far behind
		// 'resume' is to be called in 'end_frame' command
		while ( m_render_frame_id != m_context_frame_id )
			m_render_thread.suspend( );

		interlocked_inc( m_render_frame_id );
	}

	m_context_thread.destroy( INFINITE );
}

void context_thread_func( void* )
{
	api::create( );

	render_context_queue::functor functor;

	while ( m_keep_alive )
	{
		m_context_queue.pop( functor );
		functor( );
	}

	while ( !m_context_queue.empty( ) )
	{
		m_context_queue.pop( functor );
		functor( );
	}

	m_device_thread.destroy( INFINITE );

	api::destroy( );
}

void device_thread_func( void* )
{
	render_device_queue::functor functor;

	resources::create( );

	while ( m_keep_alive )
	{
		m_device_queue.pop( functor );
		functor( );
	}

	while ( !m_device_queue.empty( ) )
	{
		m_device_queue.pop( functor );
		functor( );
	}

	resources::destroy( );
}

void create( HWND in_hwnd, math::u16x2 in_resolution, bool in_is_windowed, bool in_allow_debug )
{
	parameters::set_hwnd		( in_hwnd );
	parameters::set_resolution	( in_resolution );
	parameters::set_windowed	( in_is_windowed );
	parameters::set_debug		( in_allow_debug );

	m_keep_alive				= true;

	m_render_thread.create( render_thread_func, 0, nullptr );
	m_context_thread.create( context_thread_func, 0, nullptr );
	m_device_thread.create( device_thread_func, 0, nullptr );

	resume( );
}

void destroy( )
{
	m_keep_alive				= false;
}

void wait_for_destruction( )
{
	m_render_thread.destroy( INFINITE );
	m_context_thread.destroy( INFINITE );
	m_device_thread.destroy( INFINITE );
}

void suspend( )
{
	m_render_thread.suspend( );
	m_context_thread.suspend( );
	m_device_thread.suspend( );
}

void resume( )
{
	m_render_thread.resume( );
	m_context_thread.resume( );
	m_device_thread.resume( );
}

render_context_queue& get_context_queue( )
{
	return m_context_queue;
}

render_device_queue& get_device_queue( )
{
	return m_device_queue;
}

} // namespace core

} // namespace render