#ifndef GUARD_RENDER_DDS_LOADER_H_INCLUDED
#define GUARD_RENDER_DDS_LOADER_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <lib/reader.h>

#include "dx12.h"
#include "dx_resource.h"
#include "gpu_uploader.h"

namespace render {
namespace dds_loader {

void load(
	lib::reader& in_reader,
	dx_resource& in_resource,
	D3D12_SHADER_RESOURCE_VIEW_DESC& in_srv_desc,
	lib::buffer_array<gpu_upload_task>& in_upload_tasks
);

} // namespace dds_loader
} // namespace render

#endif // #ifndef GUARD_RENDER_DDS_LOADER_H_INCLUDED