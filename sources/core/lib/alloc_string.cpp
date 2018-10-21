#include "alloc_string.h"

pcstr alloc_string::c_str( ) const
{
	return m_data;
}