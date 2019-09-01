#include "handles.h"

#include "texture.h"
#include "mesh.h"
#include "model_mesh.h"

namespace render {

DEFINE_SHARED_RESOURCE_HANDLE( mesh, u16, -1 )
DEFINE_SHARED_RESOURCE_HANDLE( texture, u16, -1 )

DEFINE_SHARED_RESOURCE_HANDLE( model_mesh, u16, -1 )

} // namespace render