#ifndef __render_shader_h_included_
#define __render_shader_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"

#include "input_layout.h"

namespace render {

class shader
{
public:
	virtual void bind( ) const = 0;
	virtual inline bool is_null( ) const = 0;
};

class vertex_shader : public shader
{
public:
	void create( pointer in_data, uptr in_size, D3D11_INPUT_ELEMENT_DESC* in_descs, u32 in_descs_count );
	void set( ID3D11VertexShader* in_shader, ID3D11InputLayout* in_layout );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	virtual void bind( ) const override;
	virtual inline bool is_null( ) const override;

protected:
	ID3D11VertexShader*	m_shader;
	ID3D11InputLayout*	m_layout;
};

class pixel_shader : public shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11PixelShader* in_shader );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	virtual void bind( ) const override;
	virtual inline bool is_null( ) const override;

protected:
	ID3D11PixelShader*	m_shader;
};

class geometry_shader : public shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11GeometryShader* in_shader );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	virtual void bind( ) const override;
	virtual inline bool is_null( ) const override;

protected:
	ID3D11GeometryShader*	m_shader;
};

class hull_shader : public shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11HullShader* in_shader );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	virtual void bind( ) const override;
	virtual inline bool is_null( ) const override;

protected:
	ID3D11HullShader*	m_shader;
};

class domain_shader : public shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11DomainShader* in_shader );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	virtual void bind( ) const override;
	virtual inline bool is_null( ) const override;

protected:
	ID3D11DomainShader*	m_shader;
};

class compute_shader : public shader
{
public:
	void create( pointer in_data, uptr in_size );
	void set( ID3D11ComputeShader* in_shader );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	virtual void bind( ) const override;
	virtual inline bool is_null( ) const override;

protected:
	ID3D11ComputeShader*	m_shader;
};

} // namespace render

#endif // #ifndef __render_shader_h_included_