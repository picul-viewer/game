#ifndef __core_alloc_string_h_included_
#define __core_alloc_string_h_included_

#include <types.h>
#include "pool.h"
#include "basic_string.h"

class alloc_string : public i_const_string<alloc_string>
{
public:
	template<typename AllocatorType>
	alloc_string( AllocatorType& allocator, const char* str );
	template<typename AllocatorType>
	alloc_string( AllocatorType& allocator, const char* str, uptr size );
	template<typename AllocatorType, typename StringClass>
	alloc_string( AllocatorType& allocator, i_const_string<StringClass> const& str );

	~alloc_string( ) = default;

	const char* c_str( ) const;

protected:
	char* m_data;
};

#include "alloc_string_inline.h"

#endif // #ifndef __core_alloc_string_h_included_
