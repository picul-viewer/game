#ifndef GUARD_UI_TEXT_EDIT_CONTROLLER_H_INCLUDED
#define GUARD_UI_TEXT_EDIT_CONTROLLER_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include <system/input.h>

namespace ui {

class text_edit_controller
{
public:
	void create( pstr const in_text_memory, uptr const in_text_memory_size );
	void destroy( );
	
	void on_char( u32 const in_key );
	void on_input( );

	pcstr get_text( ) const;
	uptr get_length( ) const;
	uptr get_selection_start( ) const;
	ptr get_selection_length( ) const;

	void clear( );

private:
	pstr m_data;
	u16 m_capacity;
	u16 m_length;
	u16 m_selection_start;
	s16 m_selection_length;

};

} // namespace ui

#endif // #ifndef GUARD_UI_TEXT_EDIT_CONTROLLER_H_INCLUDED