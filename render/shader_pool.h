#ifndef __render_shader_pool_h_included_
#define __render_shader_pool_h_included_

#include <core/types.h>
#include <core/std.h>
#include "dx_include.h"
#include "shader.h"

namespace render {

struct shader_configuration
{
public:
	inline shader_configuration( ) :
		m_data( 0 )
	{ }
	
	inline void set_vertex_type( vertex_type in_vertex_type )
	{
		ASSERT	( in_vertex_type < vertex_type_count )
		m_data	&= 0xFFFFFF00;
		m_data	|= in_vertex_type;
	}

	inline void set( u32 in_macro, bool in_value )
	{
		ASSERT	( in_macro < 32 );
		m_data	= in_value ?
			( m_data | ( 1 << in_macro ) ) :
			( m_data & ( ~( 1 << in_macro ) ) );
	}

	inline operator u32( ) { return m_data; }

protected:
	u32	m_data;
};

static const u32 c_shader_name_max_length = Cache_Line - 2 * sizeof(uptr);

class shader_pool
{
public:
	// Let's fit in cache line
	typedef fixed_string<c_shader_name_max_length> shader_name_string;

	vertex_shader*		create_vertex(		pcstr in_filename, u32 in_configuration );
	pixel_shader*		create_pixel(		pcstr in_filename, u32 in_configuration );
	geometry_shader*	create_geometry(	pcstr in_filename, u32 in_configuration );
	hull_shader*		create_hull(		pcstr in_filename, u32 in_configuration );
	domain_shader*		create_domain(		pcstr in_filename, u32 in_configuration );
	compute_shader*		create_compute(		pcstr in_filename, u32 in_configuration );

	void free_vertex	( vertex_shader* in_resource	);
	void free_pixel		( pixel_shader* in_resource		);
	void free_geometry	( geometry_shader* in_resource	);
	void free_hull		( hull_shader* in_resource		);
	void free_domain	( domain_shader* in_resource	);
	void free_compute	( compute_shader* in_resource	);

protected:
	template<typename T>
	using shader_map = map<shader_name_string, T>;

	template<typename T>
	T* create_shader( pcstr in_filename, u32 in_configuration, shader_map<T>& in_collection );

	shader_map<vertex_shader>	m_vertex_shader_data;
	shader_map<pixel_shader>	m_pixel_shader_data;
	shader_map<geometry_shader>	m_geometry_shader_data;
	shader_map<hull_shader>		m_hull_shader_data;
	shader_map<domain_shader>	m_domain_shader_data;
	shader_map<compute_shader>	m_compute_shader_data;
};

} // namespace render

#endif // #ifndef __render_shader_pool_h_included_