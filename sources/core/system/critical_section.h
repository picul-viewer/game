#ifndef GUARD_CORE_CRITICAL_SECTION_H_INCLUDED
#define GUARD_CORE_CRITICAL_SECTION_H_INCLUDED

#include <types.h>

namespace sys {

class critical_section
{
public:
	void create( );
	void destroy( );

	void enter( ) const;
	bool try_enter( ) const;

	void release( ) const;

private:
	enum : uptr { data_size = 40 };

	u8 m_data[data_size];

};

} // namespace sys

#endif // #ifndef GUARD_CORE_CRITICAL_SECTION_H_INCLUDED