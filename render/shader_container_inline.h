#ifndef __render_shader_container_inline_h_included_
#define __render_shader_container_inline_h_included_

#include <macros.h>
#include <lib/memory.h>
#include <system/file.h>

#include "shader.h"

namespace render {

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
shader_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::shader_container( )
{
#ifdef DEBUG
	m_created = false;

	memory::zero( m_shaders, ShaderEnumeratorMax * sizeof(ShaderType) );
#endif // #ifdef DEBUG
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
void shader_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::create( )
{
	ASSERT( m_created == false );

	uptr const max_shader_size = 128 * Kb;
	pvoid const memory = alloca( max_shader_size );

	for ( uptr i = 0; i < ShaderEnumeratorMax; ++i )
	{
		sys::file f( ShaderPathProvider::get( (ShaderEnumerator)i ), sys::file::open_read );

		uptr shader_size = f.size( );
		ASSERT( shader_size < max_shader_size );
		f.read( memory, shader_size );

		f.close( );

		m_shaders[i].create( memory, shader_size );
	}

#ifdef DEBUG
	m_created = true;
#endif // #ifdef DEBUG
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
void shader_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::destroy( )
{
	ASSERT( m_created == true );

	for ( uptr i = 0; i < ShaderEnumeratorMax; ++i )
		m_shaders[i].destroy( );
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
ShaderType shader_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::operator[]( uptr in_index )
{
	ASSERT( m_created == true );
	ASSERT( in_index < ShaderEnumeratorMax );

	return m_shaders[in_index];
}

} // namespace render

#endif // #ifndef __render_shader_container_inline_h_included_