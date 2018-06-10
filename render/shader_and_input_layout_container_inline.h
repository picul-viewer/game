#ifndef __render_shader_and_input_layout_container_inline_h_included_
#define __render_shader_and_input_layout_container_inline_h_included_

#include "shader_and_input_layout_container.h"

#include <lib/weak_string.h>
#include <lib/memory.h>
#include <system/file.h>

namespace render {

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::shader_and_input_layout_container( )
{
#ifdef DEBUG
	m_created = false;

	memory::zero( m_shaders, ShaderEnumeratorMax * sizeof(ShaderType) );
#endif // #ifdef DEBUG
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
void shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::create( )
{
	ASSERT( m_created == false );

	uptr const max_shader_size = 128 * Kb;
	pvoid const memory = alloca( max_shader_size );

	for ( uptr i = 0; i < ShaderEnumeratorMax; ++i )
	{
		weak_const_string path = weak_const_string( ShaderPathProvider::get( (ShaderEnumerator)i ) );
		uptr length = path.length( );

		// Assume that vertex type defined in LSByte of configuration.
		u8 vtype = 0;

		ASSERT( length > 2 );

		{
			char c = path[length - 2];
			if ( ( c >= '0' ) && ( c <= '9' ) )
				vtype += 16 * ( c - '0' );
			else
			{
				ASSERT( ( c >= 'A' ) && ( c <= 'F' ) );
				vtype += 16 * ( c - 'A' + 10 );
			}
		}

		{
			char c = path[length - 1];
			if ( ( c >= '0' ) && ( c <= '9' ) )
				vtype += c - '0';
			else
			{
				ASSERT( ( c >= 'A' ) && ( c <= 'F' ) );
				vtype += c - 'A' + 10;
			}
		}

		sys::file f( path.c_str( ), sys::file::open_read );

		uptr shader_size = f.size( );
		ASSERT( shader_size < max_shader_size );
		f.read( memory, shader_size );

		f.close( );

		m_shaders[i].create( memory, shader_size,
							 get_vertex_type_desc( (vertex_type)vtype ),
							 get_vertex_type_desc_size( (vertex_type)vtype ) );
	}

#ifdef DEBUG
	m_created = true;
#endif // #ifdef DEBUG
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
void shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::destroy( )
{
	ASSERT( m_created == true );

	for ( uptr i = 0; i < ShaderEnumeratorMax; ++i )
		m_shaders[i].destroy( );
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax, typename ShaderPathProvider>
ShaderType shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax, ShaderPathProvider>::operator[]( uptr in_index )
{
	ASSERT( m_created == true );
	ASSERT( in_index < ShaderEnumeratorMax );

	return m_shaders[in_index];
}

} // namespace render

#endif // #ifndef __render_shader_and_input_layout_container_inline_h_included_