#include "critical_section.h"
#include <Windows.h>

void sys::critical_section::create( )
{
	static_assert( sizeof(CRITICAL_SECTION) == data_size, "wrong data size" );

	InitializeCriticalSection( (LPCRITICAL_SECTION)m_data );
}

void sys::critical_section::destroy( )
{
	DeleteCriticalSection( (LPCRITICAL_SECTION)m_data );
}

void sys::critical_section::enter( ) const
{
	EnterCriticalSection( (LPCRITICAL_SECTION)m_data );
}

bool sys::critical_section::try_enter( ) const
{
	BOOL const result = TryEnterCriticalSection( (LPCRITICAL_SECTION)m_data );
	return result != FALSE;
}

void sys::critical_section::release( ) const
{
	LeaveCriticalSection( (LPCRITICAL_SECTION)m_data );
}