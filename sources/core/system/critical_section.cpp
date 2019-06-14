#include "critical_section.h"
#include <Windows.h>

void sys::critical_section::create( )
{
	static_assert( sizeof(CRITICAL_SECTION) == data_size, "wrong data size" );

	LPCRITICAL_SECTION const section = (LPCRITICAL_SECTION)m_data;

	InitializeCriticalSection( section );
}

void sys::critical_section::destroy( )
{
	LPCRITICAL_SECTION const section = (LPCRITICAL_SECTION)m_data;

	DeleteCriticalSection( section );
}

void sys::critical_section::enter( ) const
{
	LPCRITICAL_SECTION const section = (LPCRITICAL_SECTION)m_data;

	EnterCriticalSection( section );
}

bool sys::critical_section::try_enter( ) const
{
	LPCRITICAL_SECTION const section = (LPCRITICAL_SECTION)m_data;

	BOOL const result = TryEnterCriticalSection( section );
	return result != FALSE;
}

void sys::critical_section::release( ) const
{
	LPCRITICAL_SECTION const section = (LPCRITICAL_SECTION)m_data;

	LeaveCriticalSection( section );
}

bool sys::critical_section::is_locked( ) const
{
	LPCRITICAL_SECTION const section = (LPCRITICAL_SECTION)m_data;

	return section->OwningThread != nullptr;
}