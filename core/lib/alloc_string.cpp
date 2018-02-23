#include "alloc_string.h"

const char* alloc_string::c_str( ) const
{
	return m_data;
}