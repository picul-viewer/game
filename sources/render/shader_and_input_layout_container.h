#ifndef __render_shader_and_input_layout_container_h_included_
#define __render_shader_and_input_layout_container_h_included_

#include <types.h>
#include <lib/reader.h>
#include "input_layout.h"

namespace render {

template<typename ShaderType, typename ShaderEnumerator, u32 ShaderEnumeratorMax>
class shader_and_input_layout_container
{
public:
	shader_and_input_layout_container( );
	
	void create( reader& in_reader );
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

#endif // #ifndef __render_shader_and_input_layout_container_h_included_