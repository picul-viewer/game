#ifndef GUARD_UI_CONSOLE_INLINE_H_INCLUDED
#define GUARD_UI_CONSOLE_INLINE_H_INCLUDED

#include <lib/strings.h>

namespace ui {

template<>
inline void console::printf( pcstr const in_mask )
{
	uptr const length = strings::length( in_mask );
	print( in_mask, length );
}

template<typename ... Args>
inline void console::printf( pcstr const in_mask, Args const& ... args )
{
	enum { max_output = 256 };

	pstr const line_start = m_text.allocate( max_output );
	uptr const chars_wrote = snprintf( line_start, max_output, in_mask, args ... );
	m_text.shrink( max_output - chars_wrote );

	update_line_endings(line_start, chars_wrote );
}

} // namespace ui

#endif // #ifndef GUARD_UI_CONSOLE_INLINE_H_INCLUDED