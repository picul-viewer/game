#ifndef GUARD_RENDER_SHADER_H_INCLUDED
#define GUARD_RENDER_SHADER_H_INCLUDED

#include <types.h>
#include "dx_include.h"

#include "input_layout.h"

namespace render {

class vertex_shader
{
public:
	void create( pointer in_data, uptr in_size, D3D11_INPUT_ELEMENT_DESC* in_descs, u32 in_descs_count );
	void set( ID3D11VertexShader* in_shader, ID3D11InputLayout* in_layout );
	void destroy( );

	void bind( ) const;
	bool is_null( ) const;

protected:
	ID3D11VertexShader*	m_shader;
	ID3D11InputLayout*	m_layout;
};

class pixel_shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11PixelShader* in_shader );
	void destroy( );

	void bind( ) const;
	bool is_null( ) const;

protected:
	ID3D11PixelShader*	m_shader;
};

class geometry_shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11GeometryShader* in_shader );
	void destroy( );

	void bind( ) const;
	bool is_null( ) const;

protected:
	ID3D11GeometryShader*	m_shader;
};

class hull_shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11HullShader* in_shader );
	void destroy( );

	void bind( ) const;
	bool is_null( ) const;

protected:
	ID3D11HullShader*	m_shader;
};

class domain_shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11DomainShader* in_shader );
	void destroy( );

	void bind( ) const;
	bool is_null( ) const;

protected:
	ID3D11DomainShader*	m_shader;
};

class compute_shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11ComputeShader* in_shader );
	void destroy( );

	void bind( ) const;
	bool is_null( ) const;

protected:
	ID3D11ComputeShader*	m_shader;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_SHADER_H_INCLUDED