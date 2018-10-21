#ifndef __core_strings_h_included_
#define __core_strings_h_included_

#include <types.h>

namespace strings {

uptr length( pcstr str );

bool empty( pcstr str );

uptr find( pcstr str, pcstr sub_str, uptr pos = 0 );
uptr find( pcstr str, char c, uptr pos = 0 );

uptr rfind( pcstr str, char c );
uptr rfind( pcstr str, char c, uptr pos );

int compare( pcstr l, pcstr r );

bool equal( pcstr l, pcstr r );
bool less( pcstr l, pcstr r );
bool greater( pcstr l, pcstr r );

void copy( pstr destination, pcstr source );
void copy_n( pstr destination, pcstr source, uptr size );

}

#endif // #ifndef __core_strings_h_included_