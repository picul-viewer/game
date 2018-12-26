#ifndef __render_shader_and_input_layout_container_inline_h_included_
#define __render_shader_and_input_layout_container_inline_h_included_

#include "shader_and_input_layout_container.h"

#include <lib/allocator.h>
#include <lib/weak_string.h>
#include <lib/memory.h>
#include <system/file.h>

namespace render {

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax>
shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax>::shader_and_input_layout_container( )
{
#ifdef DEBUG
	m_created = false;

	memory::zero( m_shaders, ShaderEnumeratorMax * sizeof(ShaderType) );
#endif // #ifdef DEBUG
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax>
void shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax>::create( reader& in_reader )
{
	ASSERT( m_created == false );

	for ( uptr i = 0; i < ShaderEnumeratorMax; ++i )
	{
		u32 const shader_size = in_reader.read<u32>( );
		vertex_type const vtype = (vertex_type)in_reader.read<u8>( );
		pvoid const data = in_reader.read_data( shader_size );

		m_shaders[i].create( data, shader_size,
							 get_vertex_type_desc( vtype ),
							 get_vertex_type_desc_size( vtype ) );
	}

#ifdef DEBUG
	m_created = true;
#endif // #ifdef DEBUG
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax>
void shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax>::destroy( )
{
	ASSERT( m_created == true );

	for ( uptr i = 0; i < ShaderEnumeratorMax; ++i )
		m_shaders[i].destroy( );
}

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax>
ShaderType shader_and_input_layout_container<ShaderType, ShaderEnumerator, ShaderEnumeratorMax>::operator[]( ShaderEnumerator const in_index )
{
	ASSERT( m_created == true );
	ASSERT( in_index < ShaderEnumeratorMax );

	return m_shaders[in_index];
}

} // namespace render

#endif // #ifndef __render_shader_and_input_layout_container_inline_h_included_