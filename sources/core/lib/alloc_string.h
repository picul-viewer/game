#ifndef GUARD_CORE_ALLOC_STRING_H_INCLUDED
#define GUARD_CORE_ALLOC_STRING_H_INCLUDED

#include <types.h>
#include "basic_string.h"

class alloc_string : public i_const_string<alloc_string>
{
public:
	template<typename AllocatorType>
	alloc_string( AllocatorType& allocator, pcstr str );
	template<typename AllocatorType, typename StringClass>
	alloc_string( AllocatorType& allocator, i_const_string<StringClass> const& str );

	template<typename AllocatorType>
	alloc_string( AllocatorType& allocator, pcstr str, uptr size );

	~alloc_string( ) = default;

	pcstr c_str( ) const;

protected:
	pstr m_data;
};

#include "alloc_string_inline.h"

#endif // #ifndef GUARD_CORE_ALLOC_STRING_H_INCLUDED
