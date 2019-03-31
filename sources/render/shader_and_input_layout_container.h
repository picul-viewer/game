#ifndef GUARD_RENDER_SHADER_AND_INPUT_LAYOUT_CONTAINER_H_INCLUDED
#define GUARD_RENDER_SHADER_AND_INPUT_LAYOUT_CONTAINER_H_INCLUDED

#include <types.h>
#include <lib/reader.h>
#include "input_layout.h"

namespace render {

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax>
class shader_and_input_layout_container
{
public:
	shader_and_input_layout_container( );
	
	void create( lib::reader& in_reader );
	void destroy( );

	ShaderType operator[]( ShaderEnumerator const in_index );

protected:
	// To avoid zero-elemets-array error.
	enum { data_size = ( ShaderEnumeratorMax == 0 ) ? 1 : ShaderEnumeratorMax };

	ShaderType m_shaders[data_size];

#ifdef DEBUG
	bool m_created;
#endif // #ifdef DEBUG

};

} // namespace render

#include "shader_and_input_layout_container_inline.h"

#endif // #ifndef GUARD_RENDER_SHADER_AND_INPUT_LAYOUT_CONTAINER_H_INCLUDED