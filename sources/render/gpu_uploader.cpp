#include "gpu_uploader.h"
#include <lib/allocator.h>
#include <lib/memory.h>
#include "dx.h"
#include "dx_command_allocator.h"
#include "dx_command_list.h"

namespace render {

void gpu_upload_task::set_source_data(
	pcvoid const in_data,
	uptr const in_data_size,
	u64 in_source_row_pitch
)
{
	data = in_data;
	data_size = in_data_size;
	source_row_pitch = in_source_row_pitch;
}

void gpu_upload_task::set_buffer_upload(
	dx_resource const in_destination,
	u32 const in_offset
)
{
	destination = in_destination;
	subresource_index = -1;
	destination_buffer_offset = in_offset;
}

void gpu_upload_task::set_texture_upload(
	dx_resource const in_destination,
	u32 const in_subresource,
	D3D12_SUBRESOURCE_FOOTPRINT const& in_footprint
)
{
	destination = in_destination;
	subresource_index = in_subresource;
	footprint = in_footprint;
}

void gpu_upload_task::set_task_context( custom_task_context const in_context )
{
	task_context = in_context;
}

bool gpu_upload_task::is_texture_copy( ) const
{
	return subresource_index != -1;
}

void gpu_upload_task::finish( )
{
	resource_system::finish_cusom_task( task_context );
}


void gpu_uploader_queue::create( pointer const in_memory, uptr const in_size )
{
	ASSERT( ( in_size & ( in_size - 1 ) ) == 0 );

	m_data = in_memory;
	m_index_mask = (u32)( in_size - 1 );

	m_pre_push_index = 0;
	m_push_index = 0;
	m_pop_index = 0;
	m_process_index = 0;
}

void gpu_uploader_queue::destroy( )
{ }

void gpu_uploader_queue::push( gpu_upload_task* const in_tasks, u32 const count )
{
	u32 const new_push_index = interlocked_add( m_pre_push_index, count );
	u32 const target_index = new_push_index - count;

	for ( u32 i = target_index, j = 0; i < new_push_index; ++i, ++j )
		m_data[i & m_index_mask] = in_tasks + j;

	SPIN_LOCK( m_push_index != target_index );
	
	m_push_index = new_push_index;
}

gpu_upload_task* gpu_uploader_queue::get( )
{
	if ( m_push_index == m_process_index )
		return nullptr;

	return m_data[m_process_index & m_index_mask];
}

gpu_upload_task* gpu_uploader_queue::process( )
{
	if ( m_push_index == m_process_index )
		return nullptr;

	return m_data[m_process_index++ & m_index_mask];
}

gpu_upload_task* gpu_uploader_queue::pop( )
{
	if ( m_process_index == m_pop_index )
		return nullptr;

	return m_data[m_pop_index++ & m_index_mask];
}

bool gpu_uploader_queue::empty( ) const
{
	return m_push_index == m_pop_index;
}


static const uptr upload_buffer_size = 16 * Mb;

void gpu_uploader::create( )
{
	SPIN_LOCK( !m_event_created );

	{
		dx_resource::cook cook;
		cook.create_buffer( upload_buffer_size, false, false, false, false );
		cook.set_heap_type( D3D12_HEAP_TYPE_UPLOAD );
		cook.set_initial_state( D3D12_RESOURCE_STATE_GENERIC_READ );

		m_staging.create( cook );
		set_dx_name( m_staging, "staging_copy_buffer" );

		D3D12_RANGE const range = { 0, 0 }; // Do not read
		DX12_CHECK_RESULT( m_staging->Map( 0, &range, (pvoid*)&m_staging_data ) );
	}

	m_fence.create( 0 );
	m_fence_value = 0;

	m_cmd_allocator.create( D3D12_COMMAND_LIST_TYPE_COPY );

	m_cmd_list.create( D3D12_COMMAND_LIST_TYPE_COPY, m_cmd_allocator, nullptr );
	DX12_CHECK_RESULT( m_cmd_list->Close( ) );
	set_dx_name( m_cmd_list, "copy cmd_list" );

	{
		uptr const immediate_queue_size = 1024;
		uptr const background_queue_size = 4096;
		gpu_upload_task* const queue_memory = virtual_allocator( ).allocate( ( immediate_queue_size + background_queue_size ) * sizeof(gpu_upload_task) );

		m_immediate_queue.create( queue_memory, immediate_queue_size );
		m_background_queue.create( queue_memory + immediate_queue_size, background_queue_size );
	}

	m_created = true;
}

void gpu_uploader::destroy( )
{
	m_created = false;

	virtual_allocator( ).deallocate( m_immediate_queue.data( ) );
	m_cmd_list.destroy( );
	m_cmd_allocator.destroy( );
	m_fence.destroy( );
	m_staging.destroy( );
}

void gpu_uploader::push_immediate_tasks( gpu_upload_task* const in_tasks, u32 const in_count )
{
	m_immediate_queue.push( in_tasks, in_count );
	m_event.set( );
}

void gpu_uploader::push_background_tasks( gpu_upload_task* const in_tasks, u32 const in_count )
{
	m_background_queue.push( in_tasks, in_count );
	m_event.set( );
}

void gpu_uploader::process_tasks( )
{
	if ( !m_created || ( m_fence->GetCompletedValue( ) < m_fence_value ) )
		return;

	while ( gpu_upload_task* const task = m_immediate_queue.pop( ) )
		task->finish( );

	while ( gpu_upload_task* const task = m_background_queue.pop( ) )
		task->finish( );

	if ( m_immediate_queue.empty( ) && m_background_queue.empty( ) )
		return;

	DX12_CHECK_RESULT( m_cmd_allocator->Reset( ) );
	DX12_CHECK_RESULT( m_cmd_list->Reset( m_cmd_allocator, nullptr ) );

	uptr staging_size = 0;

	gpu_upload_task* task;

	while ( task = m_immediate_queue.process( ) )
	{
		bool const result = process_task( task, staging_size );
		ASSERT( result );
	}

	while ( task = m_background_queue.get( ) )
	{
		bool const result = process_task( task, staging_size );
		if ( !result )
			break;

		m_background_queue.process( );
	}

	DX12_CHECK_RESULT( m_cmd_list->Close( ) );

	ASSERT( !m_background_queue.get( ) || m_background_queue.get( )->data_size <= upload_buffer_size );
	ASSERT( staging_size != 0 );

	++m_fence_value;
	m_event.reset( );
	DX12_CHECK_RESULT( m_fence->SetEventOnCompletion( m_fence_value, m_event.get_handle( ) ) );

	g_dx.queue_copy( )->ExecuteCommandLists( 1, (ID3D12CommandList**)&m_cmd_list );
	DX12_CHECK_RESULT( g_dx.queue_copy( )->Signal( m_fence, m_fence_value ) );
}

bool gpu_uploader::process_task( gpu_upload_task* const in_task, uptr& buffer_size )
{
	if ( in_task->is_texture_copy( ) )
	{
		buffer_size = math::align_up( buffer_size, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT );

		// Assume copy is not empty.
		if ( buffer_size >= upload_buffer_size )
			return false;
	}

	uptr const copy_offset = buffer_size;

	if ( in_task->source_row_pitch != 0 )
	{
		pbyte const data = (pbyte)in_task->data;
		u64 const data_size = in_task->data_size;

		u64 const source_row_pitch = in_task->source_row_pitch;
		u64 const intermediate_row_pitch = in_task->footprint.RowPitch;

		ASSERT( data_size % source_row_pitch == 0 );
		ASSERT( source_row_pitch <= intermediate_row_pitch );

		u64 const size = data_size / source_row_pitch * intermediate_row_pitch;
		u64 const aligned_size = in_task->is_texture_copy( ) ? math::align_up( size, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT ) : size;

		if ( buffer_size + aligned_size > upload_buffer_size )
			return false;

		u64 source_offset = 0;
		u64 intermediate_offset = 0;

		for ( ; source_offset < data_size; source_offset += source_row_pitch, intermediate_offset += intermediate_row_pitch )
			memory::copy( m_staging_data + buffer_size + intermediate_offset, data + source_offset, source_row_pitch );

		buffer_size += aligned_size;
	}
	else
	{
		u64 const size = in_task->data_size;
		u64 const aligned_size = in_task->is_texture_copy( ) ? math::align_up( size, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT ) : size;

		if ( buffer_size + aligned_size > upload_buffer_size )
			return false;

		memory::copy( m_staging_data + buffer_size, in_task->data, size );

		buffer_size += aligned_size;
	}

	if ( in_task->is_texture_copy( ) )
	{
		D3D12_TEXTURE_COPY_LOCATION src_copy_location;
		src_copy_location.pResource = m_staging;
		src_copy_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src_copy_location.PlacedFootprint.Offset = copy_offset;
		src_copy_location.PlacedFootprint.Footprint = in_task->footprint;

		D3D12_TEXTURE_COPY_LOCATION dst_copy_location;
		dst_copy_location.pResource = in_task->destination;
		dst_copy_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst_copy_location.SubresourceIndex = in_task->subresource_index;

		m_cmd_list->CopyTextureRegion( &dst_copy_location, 0, 0, 0, &src_copy_location, nullptr );
	}
	else
	{
		m_cmd_list->CopyBufferRegion(
			in_task->destination, in_task->destination_buffer_offset,
			m_staging, copy_offset,
			in_task->data_size
		);
	}

	return true;
}

void gpu_uploader::copy_thread_job( u32 const in_thread_index, volatile bool& in_alive_flag )
{
	m_event.create( false, false );
	m_event_created = true;

	sys::system_event const events_to_wait[] = { m_event, resource_system::queue_event( in_thread_index ), resource_system::helper_queue_event( ) };

	while ( in_alive_flag )
	{
		process_tasks( );

		if ( resource_system::process_task( in_thread_index ) )
			continue;

		if ( resource_system::process_helper_task( ) )
			continue;

		sys::system_event::wait_any( (u32)array_size( events_to_wait ), events_to_wait );
	}

	m_event.destroy( );
}

gpu_uploader g_gpu_uploader;

} // namespace render