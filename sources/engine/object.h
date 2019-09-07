#ifndef GUARD_ENGINE_OBJECT_H_INCLUDED
#define GUARD_ENGINE_OBJECT_H_INCLUDED

#include <types.h>
#include <math/matrix.h>
#include <lib/reader.h>
#include <resource_system/default_resource.h>
#include <utils/engine_threads.h>

#include <render/object.h>

namespace engine {

class object : public default_resource<object>
{
public:
	friend class object_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_count
	};

	static void destroy_resource( object* const in_resource );

public:
	void update( math::float4x3 const& in_transform );

	inline render::object& render_object( ) { return m_render; }

private:
	static void on_subsystems_destroyed( pointer const in_resource );

private:
	friend class scene;
	void set_scene( scene* const in_scene );
	scene* get_scene( ) const;

private:
	// Aligned by 64 bytes
	math::float4x3			m_transform;

	render::object			m_render;

	//physics::game_object	m_physic;
	// Or something to handle physics...
	
	// Something to handle sound...

	scene*					m_scene;

public:
	object*					m_next;
};

} // namespace engine

#endif // #ifndef GUARD_ENGINE_OBJECT_H_INCLUDED