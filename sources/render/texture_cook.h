#ifndef GUARD_RENDER_TEXTURE_COOK_H_INCLUDED
#define GUARD_RENDER_TEXTURE_COOK_H_INCLUDED

#include <types.h>

#include <resource_system/api.h>
#include <resource_system/queried_resources.h>
#include <resource_system/shared_resource_cook.h>
#include <resources/raw_data.h>
#include <utils/engine_threads.h>

#include "gpu_uploader.h"

namespace render {

class texture;

class texture_cook : public shared_resource_cook<texture, texture_cook>
{
public:
	texture_cook( pcstr const in_path );
	static uptr size( pcstr const in_path );

public:
	enum : u32 {
		create_thread_index = engine_thread_main
	};

	void create_resource( );

private:
	u32 create( pcstr const in_path );

private:
	void query_file( texture* const in_resource_ptr );
	void on_file_loaded( queried_resources& in_queried );
	void on_gpu_upload_finished( );

private:
	enum { max_upload_tasks = 16 };

private:
	gpu_upload_task m_upload_tasks[max_upload_tasks];
	raw_data::ptr m_raw_data;
	texture* m_result;
	uptr m_length;

#pragma warning (push)
#pragma warning (disable:4200)
	char m_path[0];
#pragma warning (pop)

};

} // namespace render

#endif // #ifndef GUARD_RENDER_TEXTURE_COOK_H_INCLUDED