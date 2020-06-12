#ifndef GUARD_RENDER_GPU_OBJECT_MANAGER_INLINE_H_INCLUDED
#define GUARD_RENDER_GPU_OBJECT_MANAGER_INLINE_H_INCLUDED

#include <macros.h>

namespace render {

template<typename T>
void gpu_object_manager<T>::create( pointer const in_memory, u32 const in_element_count )
{
	static_assert( max_frame_delay == 2, "rework the code" );

	m_element_count = in_element_count;

	u32 const allocator_memory_size = gpu_double_pool_allocator::memory_size( in_element_count );
	u32 const cpu_memory_size = in_element_count * sizeof(T);

	pointer m = in_memory;

	m_allocator.create( m, in_element_count, 0, 0 );
	m += allocator_memory_size;
	m_cpu_data = m;
	m += cpu_memory_size;
	m_cpu_data_staging = m;
	m += cpu_memory_size;

	m_update_bounds = math::u32x4( in_element_count, 0, in_element_count, 0 );

	ASSERT( m == in_memory + memory_size( in_element_count ) );

	{
		dx_resource::cook cook;
		cook.create_buffer(
			in_element_count * sizeof(T),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_buffers[i].create( cook );
			set_dx_name( m_buffers[i], format( "buffer #%d", i ) );
		}
	}
}

template<typename T>
void gpu_object_manager<T>::destroy( )
{
	for ( u32 i = 0; i < max_frame_delay; ++i )
		m_buffers[i].destroy( );
}

template<typename T>
u32 gpu_object_manager<T>::allocate_static( )
{
	return m_allocator.allocate_left( );
}

template<typename T>
u32 gpu_object_manager<T>::allocate_dynamic( )
{
	return m_allocator.allocate_right( );
}

template<typename T>
void gpu_object_manager<T>::deallocate( u32 const in_handle )
{
	m_allocator.deallocate( in_handle );
}

template<typename T>
void gpu_object_manager<T>::get_init_tasks( u32 const in_handle, T& data, lib::buffer_array<gpu_upload_task>& in_tasks )
{
	// Static only
	ASSERT_CMP( in_handle, <, m_allocator.left_last_index( ) );

	for ( u32 i = 0; i < max_frame_delay; ++i )
	{
		gpu_upload_task& task	= in_tasks.emplace_back( );
		task.set_source_data	( &data, sizeof(T) );
		task.set_buffer_upload	( m_buffers[i], sizeof(T) * in_handle );
	}
}

template<typename T>
void gpu_object_manager<T>::update( u32 const in_handle, T const& data )
{
	// Dynamic transforms only
	ASSERT_CMP( in_handle, >=, m_allocator.right_last_index( ) );

	m_cpu_data[in_handle] = data;

	m_update_bounds.x = math::min( m_update_bounds.x, in_handle );
	m_update_bounds.y = math::max( m_update_bounds.y, in_handle + 1 );
}

template<typename T>
bool gpu_object_manager<T>::need_gpu_update( ) const
{
	return ( m_update_bounds.x < m_update_bounds.y ) || ( m_update_bounds.z < m_update_bounds.w );
}

template<typename T>
void gpu_object_manager<T>::update_gpu_task( gpu_upload_task& in_task )
{
	ASSERT( need_gpu_update( ) );

	math::u32x2 const bounds = ( m_update_bounds.z < m_update_bounds.w ) ?
		math::u32x2(
			math::min( m_update_bounds.x, m_update_bounds.z ),
			math::max( m_update_bounds.y, m_update_bounds.w )
		) :
		m_update_bounds.vx.vx;

	ASSERT_CMP( bounds.x, >=, m_allocator.right_last_index( ) );
	ASSERT_CMP( bounds.y, <=, m_element_count );

	pvoid const data_ptr = m_cpu_data + bounds.x;
	pvoid const staging_data_ptr = m_cpu_data_staging + bounds.x;
	uptr const data_size = sizeof(T) * ( bounds.y - bounds.x );

	memory::copy( staging_data_ptr, data_ptr, data_size );

	u32 const buffer_index = g_render.frame_index( ) % max_frame_delay;

	in_task.set_source_data		( staging_data_ptr, data_size );
	in_task.set_buffer_upload	( m_buffers[buffer_index], sizeof(T) * bounds.x );

	m_update_bounds = math::u32x4( m_element_count, 0, m_update_bounds.x, m_update_bounds.y );
}

template<typename T>
u32 gpu_object_manager<T>::memory_size( u32 const in_element_count )
{
	u32 const allocator_memory_size = gpu_double_pool_allocator::memory_size( in_element_count );
	u32 const cpu_memory_size = in_element_count * sizeof(T);

	return allocator_memory_size + 2 * cpu_memory_size;
}

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_OBJECT_MANAGER_INLINE_H_INCLUDED