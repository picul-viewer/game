#ifndef __core_alloc_string_h_included_
#define __core_alloc_string_h_included_

#include <core/types.h>
#include "pool.h"
#include "basic_string.h"

class alloc_string : public i_const_string
{
public:
	template<typename AllocatorType>
	alloc_string( AllocatorType& allocator, const char* str );
	template<typename AllocatorType>
	alloc_string( AllocatorType& allocator, const char* str, uptr size );
	template<typename AllocatorType>
	alloc_string( AllocatorType& allocator, i_const_string const& str );

	~alloc_string( ) = default;

	virtual const char* c_str( ) const override;

protected:
	char* m_data;
};

#include "alloc_string_inline.h"

#endif // #ifndef __core_alloc_string_h_included_
