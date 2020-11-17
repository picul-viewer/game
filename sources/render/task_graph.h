#ifndef GUARD_RENDER_TASK_GRAPH_H_INCLUDED
#define GUARD_RENDER_TASK_GRAPH_H_INCLUDED

#include <types.h>
#include "dx12.h"
#include "dx_command_list.h"
#include "dx_resource.h"

namespace render {

class task_graph
{
public:
	void create( );
	void destroy( );

	void register_resource( dx_resource& in_resource, D3D12_RESOURCE_STATES const in_initial_state );

	void begin_usage( dx_resource& in_resource, D3D12_RESOURCE_STATES const in_necessary_state );
	void end_usage( dx_resource& in_resource );

	void submit_barriers( );

private:
	dx_command_list& m_cmd_list;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_TASK_GRAPH_H_INCLUDED