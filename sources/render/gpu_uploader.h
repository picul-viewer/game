#ifndef GUARD_RENDER_GPU_UPLOADER_H_INCLUDED
#define GUARD_RENDER_GPU_UPLOADER_H_INCLUDED

#include <types.h>
#include <math/vector.h>
#include <resource_system/api.h>
#include <system/mpsc_queue.h>

#include "dx_command_allocator.h"
#include "dx_command_list.h"
#include "dx_fence.h"
#include "dx_resource.h"

namespace render {

class gpu_upload_task
{
public:
	void set_source_data(
		pcvoid const in_data,
		uptr const in_data_size,
		u64 in_source_row_pitch = 0
	);

	void set_buffer_upload(
		dx_resource const in_destination,
		u32 const in_offset
	);
	void set_texture_upload(
		dx_resource const in_destination,
		u32 const in_subresource,
		D3D12_SUBRESOURCE_FOOTPRINT const& in_footprint
	);

	void set_task_context( custom_task_context const in_context );

	bool is_texture_copy( ) const;

private:
	friend class gpu_uploader;

	void finish( );

private:
	D3D12_SUBRESOURCE_FOOTPRINT footprint;
	custom_task_context task_context;
	pcvoid data;
	uptr data_size;
	u64 source_row_pitch;
	dx_resource destination;
	u64 destination_buffer_offset;
	u32 subresource_index;

};

class gpu_uploader_queue
{
public:
	void create( pointer const in_memory, uptr const in_size );
	void destroy( );

	void push( gpu_upload_task* const in_tasks, u32 const count );
	gpu_upload_task* get( );
	gpu_upload_task* process( );
	gpu_upload_task* pop( );

	inline pointer data( ) const { return m_data; }
	bool empty( ) const;

private:
	union
	{
		mt_u32 m_pre_push_index;
		u8 m_padding0[Cache_Line];
	};
	union
	{
		mt_u32 m_push_index;
		u8 m_padding1[Cache_Line];
	};
	union
	{
		struct
		{
			u32 m_pop_index;
			u32 m_process_index;
		};
		u8 m_padding2[Cache_Line];
	};
	gpu_upload_task** m_data;
	u32 m_index_mask;
};

class gpu_uploader
{
public:
	void create( );
	void destroy( );

	void push_immediate_tasks( gpu_upload_task* const in_tasks, u32 const in_count );
	void push_background_tasks( gpu_upload_task* const in_tasks, u32 const in_count );

	void copy_thread_job( u32 const in_thread_index, volatile bool& in_alive_flag );

private:
	void process_tasks( );
	bool process_task( gpu_upload_task* const in_task, uptr& buffer_size );

private:
	gpu_uploader_queue m_immediate_queue;
	gpu_uploader_queue m_background_queue;
	sys::system_event m_event;
	dx_resource m_staging;
	pbyte m_staging_data;
	dx_command_allocator m_cmd_allocator;
	dx_command_list m_cmd_list;
	dx_fence m_fence;
	u64 m_fence_value;
	volatile bool m_event_created = false;
	volatile bool m_created = false;

};

extern gpu_uploader g_gpu_uploader;

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_UPLOADER_H_INCLUDED