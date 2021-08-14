#ifndef GUARD_RENDER_RENDER_RESOURCES_H_INCLUDED
#define GUARD_RENDER_RENDER_RESOURCES_H_INCLUDED

#include <types.h>
#include "dx12.h"
#include "dx_resource.h"
#include "dx_views.h"

namespace render {

class render_target_2d
{
public:
	void create(
		pcstr const in_name,
		DXGI_FORMAT in_format,
		math::u32x2 const in_size,
		D3D12_RESOURCE_STATES const in_initial_state,
		math::float4 const in_clear_value = math::float4( 0.0f, 0.0f, 0.0f, 0.0f )
	);

	void destroy( );

	inline D3D12_CPU_DESCRIPTOR_HANDLE rtv( ) const { return m_rtv; }
	dx_srv_cook srv_cook( ) const;

	inline dx_resource resource( ) const { return m_resource; }
	inline operator dx_resource( ) const { return m_resource; }

private:
	dx_resource m_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtv;
	DXGI_FORMAT m_format;

};

class depth_stencil_2d
{
public:
	void create(
		pcstr const in_name,
		DXGI_FORMAT in_format,
		math::u32x2 const in_size,
		D3D12_RESOURCE_STATES const in_initial_state,
		float const in_clear_depth = 0.0f,
		u8 const in_clear_stencil = 0,
		bool const in_need_srv = false,
		bool const in_need_read_only_depth = false
	);

	void destroy( );

	inline dx_resource resource( ) const { return m_resource; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE dsv( ) const { return m_dsv; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE read_only_dsv( ) const { return m_read_only_dsv; }
	dx_srv_cook srv_cook( ) const;

	inline operator dx_resource( ) const { return m_resource; }

private:
	dx_resource m_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_read_only_dsv;
	DXGI_FORMAT m_format;

};

class compute_target_2d
{
public:
	void create(
		pcstr const in_name,
		DXGI_FORMAT in_format,
		math::u32x2 const in_size,
		D3D12_RESOURCE_STATES const in_initial_state
	);

	void destroy( );

	dx_uav_cook uav_cook( ) const;
	dx_srv_cook srv_cook( ) const;

	inline dx_resource resource( ) const { return m_resource; }
	inline operator dx_resource( ) const { return m_resource; }

private:
	dx_resource m_resource;
	DXGI_FORMAT m_format;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_RESOURCES_H_INCLUDED