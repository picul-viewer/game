#ifndef __core_alloc_string_h_included_
#define __core_alloc_string_h_included_

#include <core/types.h>
#include "heap.h"
#include "basic_string.h"

static const uptr alloc_string_heap_page_size = 8192;

template<u32 PageSize>
using alloc_string_heap = dynamic_allocation_heap<PageSize>;

class alloc_string : public i_const_string
{
public:
	template<u32 HeapPageSize>
	alloc_string( alloc_string_heap<HeapPageSize>& heap, const char* str );
	template<u32 HeapPageSize>
	alloc_string( alloc_string_heap<HeapPageSize>& heap, const char* str, uptr size );
	template<u32 HeapPageSize>
	alloc_string( alloc_string_heap<HeapPageSize>& heap, i_const_string const& str );

	~alloc_string( ) = default;

	virtual const char* c_str( ) const override;

protected:
	char* m_data;
};

#include "alloc_string_inline.h"

#endif // #ifndef __core_alloc_string_h_included_
