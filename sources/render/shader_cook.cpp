#include "shader_cook.h"
#include <resource_system/api.h>
#include <resources/resources_path.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

namespace render {

shader_cook* shader_cook::create(
	shader_type const in_type,
	pcstr const in_name,
	u32 const in_defines_count,
	shader_define* const in_defines
)
{
	uptr const size = sizeof(shader_cook) + in_defines_count * sizeof(shader_define);
	shader_cook* const result = std_allocator( ).allocate( size );
	result->init( );
	result->m_type = in_type;
	result->m_name = in_name;
	result->m_defines_count = in_defines_count;
	if ( in_defines != nullptr )
		memory::copy( result->m_defines, in_defines, in_defines_count * sizeof(shader_define) );

	return result;
}

void shader_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void shader_cook::create_resource( )
{
	m_source_path = GET_RESOURCE_SOURCE_PATH( "shaders\\" );
	m_source_path += m_name;

	raw_data_cook* const cook = raw_data_cook::create( m_source_path.c_str( ) );

	create_child_resources(
		resource_system::user_callback_task<shader_cook, &shader_cook::compile_shader>( this ), cook
	);
}

void shader_cook::compile_shader( queried_resources& in_resources )
{
	raw_data::ptr data = in_resources.get_resource<raw_data::ptr>( );

	D3D_SHADER_MACRO* const macros = stack_allocate( ( m_defines_count + 1 ) * sizeof(D3D_SHADER_MACRO) );

	for ( u32 i = 0; i < m_defines_count; ++i )
	{
		macros[i].Name = m_defines[i].name;
		macros[i].Definition = m_defines[i].value;
	}
	macros[m_defines_count].Name = nullptr;
	macros[m_defines_count].Definition = nullptr;

	pcstr const targets[shader_type_count] = {
		"vs_5_1", "hs_5_1", "ds_5_1", "gs_5_1", "ps_5_1", "cs_5_1"
	};
	pcstr const target = targets[m_type];

	u32 flags =
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR |
		D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
		D3DCOMPILE_ALL_RESOURCES_BOUND;
		
#ifdef DEBUG
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif // #ifdef DEBUG

	ID3DBlob* code;
	ID3DBlob* errors;

	HRESULT const result = D3DCompile(
		data->data( ), data->size( ), m_source_path.c_str( ), macros,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
		target, flags, 0, &code, &errors
	);

	if ( SUCCEEDED( result ) )
	{
		if ( errors )
		{
			LOG(
				"shader_cook: warnings occured when compiling shader: \"%s\"\n"
				"%s"
				"-----------------------------------------------------------------------------------\n",
				m_name, (pcstr)errors->GetBufferPointer( )
			);

			errors->Release( );
		}
		else
			LOG( "shader_cook: shader compiled successfully: \"%s\"\n" , m_name );

		ASSERT( code );

		pvoid const output_data = code->GetBufferPointer( );
		uptr const output_size = code->GetBufferSize( );

		raw_data* const output = raw_data::create( output_size );
		memory::copy( output->data( ), output_data, output_size );

		code->Release( );

		finish( output );
	}
	else
	{
		if ( errors )
		{
			LOG(
				"shader_cook: errors occured when compiling shader: \"%s\"\n"
				"%s"
				"-----------------------------------------------------------------------------------\n",
				m_name, (pcstr)errors->GetBufferPointer( )
			);
					
			errors->Release( );
		}
		else
			LOG( "shader_cook: unknown error when compiling shader: \"%s\"\n", m_name );
				
		if ( code )
			code->Release( );

		finish( nullptr );
	}
}

} // namespace render