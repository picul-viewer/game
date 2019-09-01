#ifndef GUARD_RENDER_DDS_LOADER_H_INCLUDED
#define GUARD_RENDER_DDS_LOADER_H_INCLUDED

#include <types.h>

#include "dx_include.h"
#include <lib/reader.h>

namespace render {
namespace dds_loader {

void load( ID3D11ShaderResourceView*& out_srv, lib::reader& in_reader );

} // namespace dds_loader
} // namespace render

#endif // #ifndef GUARD_RENDER_DDS_LOADER_H_INCLUDED