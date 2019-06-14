#include "conditional_variable.h"
#include <Windows.h>

void sys::conditional_variable::create( )
{
	static_assert( sizeof(CONDITION_VARIABLE) == data_size, "wrong data size" );

	PCONDITION_VARIABLE const data = (PCONDITION_VARIABLE)m_data;

	InitializeConditionVariable( data );
}

void sys::conditional_variable::destroy( )
{ }

void sys::conditional_variable::sleep( critical_section const& cs, u32 const wait_ms ) const
{
	PCONDITION_VARIABLE const data = (PCONDITION_VARIABLE)m_data;
	PCRITICAL_SECTION const cs_data = (PCRITICAL_SECTION)cs.m_data;

	SleepConditionVariableCS( data, cs_data, wait_ms );
}

void sys::conditional_variable::wake( ) const
{
	PCONDITION_VARIABLE const data = (PCONDITION_VARIABLE)m_data;

	WakeConditionVariable( data );
}

void sys::conditional_variable::wake_all( ) const
{
	PCONDITION_VARIABLE const data = (PCONDITION_VARIABLE)m_data;

	WakeAllConditionVariable( data );
}