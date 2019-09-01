#ifndef GUARD_UI_FONT_COOK_H_INCLUDED
#define GUARD_UI_FONT_COOK_H_INCLUDED

#include <types.h>

#include <lib/reader.h>

#include <resource_system/queried_resources.h>
#include <resource_system/shared_resource_cook.h>
#include <utils/engine_threads.h>

namespace ui {

class font;

class font_cook : public shared_resource_cook<font, font_cook>
{
public:
	static font_cook* create( pcstr const in_path );
	static void destroy( pointer const in_cook );

public:
	enum : u32 {
		create_thread_index = engine_helper_thread_0
	};

	void create_resource( );

private:
	void query_file( font* const in_resource_ptr );
	void on_file_loaded( queried_resources& in_queried );
	void on_texture_ready( queried_resources& in_queried );

private:
	font* m_result;
	uptr m_length;

#pragma warning (push)
#pragma warning (disable:4200)
	char m_path[0];
#pragma warning (pop)

};

} // namespace ui

#endif // #ifndef GUARD_UI_FONT_COOK_H_INCLUDED