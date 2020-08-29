#ifndef GUARD_CORE_TYPES_H_INCLUDED
#define GUARD_CORE_TYPES_H_INCLUDED

#include <cstddef>
#include <new>
#include <utility>

typedef char				s8;
typedef short				s16;
typedef int					s32;
typedef long long			s64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef float				f32;
typedef double				f64;

typedef intptr_t			ptr;
typedef uintptr_t			uptr;

typedef unsigned char*		pbyte;
typedef unsigned char const*pcbyte;

typedef char*				pstr;
typedef char const*			pcstr;

typedef void*				pvoid;
typedef void const*			pcvoid;

enum
{
	Cache_Line = 64,
	Memory_Page_Size = 4096
};

enum : ptr
{
	Kb	= 1024LL,
	Mb	= 1024LL * Kb,
	Gb	= 1024LL * Mb
};

enum : u32
{
	wait_infinite = 0xFFFFFFFF
};


template<typename ... TList>
struct type_count;

template<>
struct type_count<>
{
	enum { value = 0 };
};

template<typename T, typename ... TList>
struct type_count<T, TList ...>
{
	enum { value = 1 + type_count<TList ...>::value };
};


template<typename T, uptr N>
static inline uptr array_size( T ( & )[N] )
{
	return N;
}


struct pointer
{
	inline pointer( ) = default;
	inline pointer( pcvoid const data ) { this->data = (pvoid)data; }

	template<typename T>
	inline operator T*( ) const { return (T*)data; }

	inline operator uptr( ) const { return (uptr)data; }

	template<typename T>
	inline pointer& operator=( T* const data ) { this->data = (pvoid)data; return *this; }

	inline pointer& operator=( uptr const data ) { this->data = (pvoid)data; return *this; }

	inline pointer& operator=( nullptr_t const data ) { this->data = (pvoid)data; return *this; }

	inline bool operator==( pointer const p ) const { return data == p.data; }
	inline bool operator!=( pointer const p ) const { return data != p.data; }
	inline bool operator>( pointer const p ) const { return data > p.data; }
	inline bool operator<( pointer const p ) const { return data < p.data; }
	inline bool operator>=( pointer const p ) const { return data >= p.data; }
	inline bool operator<=( pointer const p ) const { return data <= p.data; }

	inline pointer operator+( u64 const n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( u64 const n ) const { return pointer( (pbyte)data - n ); }
	inline pointer operator+( s64 const n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( s64 const n ) const { return pointer( (pbyte)data - n ); }
	inline pointer operator+( u32 const n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( u32 const n ) const { return pointer( (pbyte)data - n ); }
	inline pointer operator+( s32 const n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( s32 const n ) const { return pointer( (pbyte)data - n ); }

	inline ptr operator-( pointer const p ) const { return (pbyte)data - (pbyte)p.data; }

	inline pointer& operator+=( uptr const n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( uptr const n ) { data = (pbyte)data - n; return *this; }
	inline pointer& operator+=( ptr const n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( ptr const n ) { data = (pbyte)data - n; return *this; }
	inline pointer& operator+=( u32 const n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( u32 const n ) { data = (pbyte)data - n; return *this; }
	inline pointer& operator+=( s32 const n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( s32 const n ) { data = (pbyte)data - n; return *this; }

	inline pointer& operator++( ) { data = (pbyte)data + 1; return *this; }
	inline pointer& operator--( ) { data = (pbyte)data - 1; return *this; }

	inline pointer operator++( int const n ) { pointer p( *this ); data = (pbyte)data + 1; return p; }
	inline pointer operator--( int const n ) { pointer p( *this ); data = (pbyte)data - 1; return p; }

	template<typename T>
	inline T& get( uptr const offset = 0 ) const { return *( (T*)data + offset ); }

	inline pointer align_down( uptr const alignment ) const { return (pvoid)( (uptr)data - (uptr)data % alignment ); }
	inline pointer align_up( uptr const alignment ) const { return ( *this + alignment - 1 ).align_down( alignment ); }

private:
	pvoid data;

};

class non_copyable
{
public:
	non_copyable( non_copyable const& ) = delete;
	non_copyable& operator=( non_copyable const& ) = delete;
	non_copyable( non_copyable&& ) = delete;
	non_copyable& operator=( non_copyable&& ) = delete;

};

namespace lib {
	using std::forward;
} // namespace lib

template<typename T>
struct raii
{
private:
	T m_data;

public:
	template<typename ... Args>
	raii( Args&& ... args )
	{
		m_data.create( lib::forward<Args>( args ) ... );
	}

	~raii( )
	{
		m_data.destroy( );
	}

	operator T const&( ) const { return (T const&)m_data; }
	operator T&( ) { return (T&)m_data; }
	T const* operator->( ) const { return (T const*)&m_data; }
	T* operator->( ) { return (T*)&m_data; }
};

template<typename T>
struct unraii
{
private:
	std::aligned_storage_t<sizeof(T), alignof(T)> m_data;

public:
	template<typename ... Args>
	void create( Args&& ... args )
	{
		new ( &m_data ) T( lib::forward<Args>( args ) ... );
	}

	void destroy( )
	{
		( (T&)m_data ).~T( );
	}

	operator T const&( ) const { return (T const&)m_data; }
	operator T&( ) { return (T&)m_data; }
	T const* operator->( ) const { return (T const*)&m_data; }
	T* operator->( ) { return (T*)&m_data; }
};

enum memory_types
{
	memory_type_allocated = 0,
	memory_type_reserved
};

#define mem_align(n) __declspec(align(n))

#define no_vtable __declspec(novtable)

#define aligned(p, n) ((uptr)p % n == 0)

template<uptr Size>
struct mem_align(1) memory_block
{
	u8 data[Size];

	template<typename T>
	operator T const&( ) const
	{
		static_assert( sizeof(T) <= Size, "invalid cast" );
		return *(T*)data;
	}

	template<typename T>
	operator T&( )
	{
		static_assert( sizeof(T) <= Size, "invalid cast" );
		return *(T*)data;
	}
};

#endif // #ifndef GUARD_CORE_TYPES_H_INCLUDED