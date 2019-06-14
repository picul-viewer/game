#ifndef GUARD_CORE_CONDITIONAL_VARIABLE_H_INCLUDED
#define GUARD_CORE_CONDITIONAL_VARIABLE_H_INCLUDED

#include <types.h>
#include "critical_section.h"

namespace sys {

class conditional_variable
{
public:
	void create( );
	void destroy( );

	void sleep( critical_section const& cs, u32 const wait_ms = wait_infinite ) const;

	void wake( ) const;
	void wake_all( ) const;

private:
	enum : uptr { data_size = 8 };

	u8 m_data[data_size];

};

} // namespace sys

#endif // #ifndef GUARD_CORE_CONDITIONAL_VARIABLE_H_INCLUDED