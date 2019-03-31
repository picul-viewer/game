#ifndef GUARD_CORE_TYPES_H_INCLUDED
#define GUARD_CORE_TYPES_H_INCLUDED

#include <cstddef>
#include <new>

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

enum : uptr
{
	Kb	= 1024LL,
	Mb	= 1024LL * Kb,
	Gb	= 1024LL * Mb
};

enum time : u32
{
	infinite = 0xFFFFFFFF
};


template<typename ... TList>
struct type_count;

template<>
struct type_count<>
{
	static const uptr value = 0;
};

template<typename T, typename ... TList>
struct type_count<T, TList ...>
{
	static const uptr value = 1 + type_count<TList ...>::value;
};


struct pointer
{
	inline pointer( ) = default;
	inline pointer( pcvoid data ) { this->data = (pvoid)data; }

	template<typename T>
	inline operator T*( ) const { return (T*)data; }
	
	inline operator uptr( ) const { return (uptr)data; }
	
	template<typename T>
	inline pointer& operator=( T* data ) { this->data = (pvoid)data; return *this; }
	
	inline pointer& operator=( uptr data ) { this->data = (pvoid)data; return *this; }

	inline pointer& operator=( nullptr_t data ) { this->data = (pvoid)data; return *this; }

	inline bool operator==( pointer p ) const { return data == p.data; }
	inline bool operator!=( pointer p ) const { return data != p.data; }
	inline bool operator>( pointer p ) const { return data > p.data; }
	inline bool operator<( pointer p ) const { return data < p.data; }
	inline bool operator>=( pointer p ) const { return data >= p.data; }
	inline bool operator<=( pointer p ) const { return data <= p.data; }

	inline pointer operator+( u64 n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( u64 n ) const { return pointer( (pbyte)data - n ); }
	inline pointer operator+( s64 n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( s64 n ) const { return pointer( (pbyte)data - n ); }
	inline pointer operator+( u32 n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( u32 n ) const { return pointer( (pbyte)data - n ); }
	inline pointer operator+( s32 n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( s32 n ) const { return pointer( (pbyte)data - n ); }

	inline ptr operator-( pointer p ) const { return (pbyte)data - (pbyte)p.data; }

	inline pointer& operator+=( uptr n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( uptr n ) { data = (pbyte)data - n; return *this; }
	inline pointer& operator+=( ptr n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( ptr n ) { data = (pbyte)data - n; return *this; }
	inline pointer& operator+=( u32 n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( u32 n ) { data = (pbyte)data - n; return *this; }
	inline pointer& operator+=( s32 n ) { data = (pbyte)data + n; return *this; }
	inline pointer& operator-=( s32 n ) { data = (pbyte)data - n; return *this; }
	
	inline pointer& operator++( ) { data = (pbyte)data + 1; return *this; }
	inline pointer& operator--( ) { data = (pbyte)data - 1; return *this; }

	inline pointer operator++( int n ) { pointer p( *this ); data = (pbyte)data + 1; return p; }
	inline pointer operator--( int n ) { pointer p( *this ); data = (pbyte)data - 1; return p; }

	template<typename T>
	inline T& get( uptr offset = 0 ) const { return *( (T*)data + offset ); }

	inline pointer align_down( uptr const alignment ) const { return (pvoid)( (uptr)data - (uptr)data % alignment ); }
	inline pointer align_up( uptr const alignment ) const { return ( *this + alignment - 1 ).align_down( alignment ); }

private:
	pvoid data;

};

#define mem_align(n) __declspec(align(n))

#define no_vtable __declspec(novtable)

#define aligned(p, n) ((uptr)p % n == 0)

#endif // #ifndef GUARD_CORE_TYPES_H_INCLUDED