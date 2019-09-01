#ifndef GUARD_RENDER_HANDLES_H_INCLUDED
#define GUARD_RENDER_HANDLES_H_INCLUDED

#include <types.h>
#include <resource_system/resource_handle.h>

namespace render {

DECLARE_SHARED_RESOURCE_HANDLE( texture, u16 )
DECLARE_SHARED_RESOURCE_HANDLE( mesh, u16 )

DECLARE_SHARED_RESOURCE_HANDLE( model_mesh, u16 )

} // namespace render

#endif // #ifndef GUARD_RENDER_HANDLES_H_INCLUDED