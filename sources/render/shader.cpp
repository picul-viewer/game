#include "shader.h"
#include "api.h"

namespace render {

void vertex_shader::create( pointer in_data, uptr in_size, D3D11_INPUT_ELEMENT_DESC* in_descs, u32 in_descs_count )
{
	g_api.get_device( )->CreateInputLayout( in_descs, in_descs_count, in_data, in_size, &m_layout );
	g_api.get_device( )->CreateVertexShader( in_data, in_size, nullptr, &m_shader );
}

void vertex_shader::set( ID3D11VertexShader* in_shader, ID3D11InputLayout* in_layout )
{
	m_shader = in_shader;
	m_layout = in_layout;
}

void vertex_shader::destroy( )
{
	m_shader->Release( );
	m_layout->Release( );
}

void vertex_shader::bind( ) const
{
	g_api.get_context( )->IASetInputLayout( m_layout );
	g_api.get_context( )->VSSetShader( m_shader, nullptr, 0 );
}

bool vertex_shader::is_null( ) const
{
	return ( m_shader == nullptr ) || ( m_layout == nullptr );
}


void pixel_shader::create( pointer in_data, uptr in_size )
{
	g_api.get_device( )->CreatePixelShader( in_data, in_size, nullptr, &m_shader );
}

void pixel_shader::set( ID3D11PixelShader* in_shader )
{
	m_shader = in_shader;
}

void pixel_shader::destroy( )
{
	m_shader->Release( );
}

void pixel_shader::bind( ) const
{
	g_api.get_context( )->PSSetShader( m_shader, nullptr, 0 );
}

bool pixel_shader::is_null( ) const
{
	return m_shader == nullptr;
}


void geometry_shader::create( pointer in_data, uptr in_size )
{
	g_api.get_device( )->CreateGeometryShader( in_data, in_size, nullptr, &m_shader );
}

void geometry_shader::set( ID3D11GeometryShader* in_shader )
{
	m_shader = in_shader;
}

void geometry_shader::destroy( )
{
	m_shader->Release( );
}

void geometry_shader::bind( ) const
{
	g_api.get_context( )->GSSetShader( m_shader, nullptr, 0 );
}

bool geometry_shader::is_null( ) const
{
	return m_shader == nullptr;
}


void hull_shader::create( pointer in_data, uptr in_size )
{
	g_api.get_device( )->CreateHullShader( in_data, in_size, nullptr, &m_shader );
}

void hull_shader::set( ID3D11HullShader* in_shader )
{
	m_shader = in_shader;
}

void hull_shader::destroy( )
{
	m_shader->Release( );
}

void hull_shader::bind( ) const
{
	g_api.get_context( )->HSSetShader( m_shader, nullptr, 0 );
}

bool hull_shader::is_null( ) const
{
	return m_shader == nullptr;
}


void domain_shader::create( pointer in_data, uptr in_size )
{
	g_api.get_device( )->CreateDomainShader( in_data, in_size, nullptr, &m_shader );
}

void domain_shader::set( ID3D11DomainShader* in_shader )
{
	m_shader = in_shader;
}

void domain_shader::destroy( )
{
	m_shader->Release( );
}

void domain_shader::bind( ) const
{
	g_api.get_context( )->DSSetShader( m_shader, nullptr, 0 );
}

bool domain_shader::is_null( ) const
{
	return m_shader == nullptr;
}


void compute_shader::create( pointer in_data, uptr in_size )
{
	g_api.get_device( )->CreateComputeShader( in_data, in_size, nullptr, &m_shader );
}

void compute_shader::set( ID3D11ComputeShader* in_shader )
{
	m_shader = in_shader;
}

void compute_shader::destroy( )
{
	m_shader->Release( );
}

void compute_shader::bind( ) const
{
	g_api.get_context( )->CSSetShader( m_shader, nullptr, 0 );
}

bool compute_shader::is_null( ) const
{
	return m_shader == nullptr;
}

} // namespace render