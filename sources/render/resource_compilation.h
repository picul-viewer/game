#ifndef GUARD_RENDER_RESOURCE_COMPILATION_H_INCLUDED
#define GUARD_RENDER_RESOURCE_COMPILATION_H_INCLUDED

#include <types.h>
#include <lib/reader.h>
#include <lib/dynamic_writer.h>
#include <utils/config_reader.h>

namespace render {

void model_mesh_serialize( utils::config_reader_node const& in_cfg, lib::dynamic_writer& in_writer );

void scene_serialize( utils::config_reader_node const& in_cfg, lib::dynamic_writer& in_writer );

} // namespace render

#endif // #ifndef GUARD_RENDER_RESOURCE_COMPILATION_H_INCLUDED