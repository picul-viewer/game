#include "task_graph.h"
#include "api.h"
#include <d3dcompiler.h>

namespace render {

void task_graph::create( )
{ }

void task_graph::destroy( )
{ }

void task_graph::register_resource( dx_resource& in_resource, D3D12_RESOURCE_STATES const in_initial_state )
{

}

void task_graph::begin_usage( dx_resource& in_resource, D3D12_RESOURCE_STATES const in_necessary_state )
{

}

void task_graph::end_usage( dx_resource& in_resource )
{

}

void task_graph::submit_barriers( )
{

}

} // namespace render