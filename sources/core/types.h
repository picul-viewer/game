#ifndef __core_types_h_included_
#define __core_types_h_included_

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
	inline pointer( pvoid data ) { this->data = data; }

	template<typename T>
	inline operator T*( ) const { return (T*)data; }
	
	inline operator bool( ) const { return data != nullptr; }
	
	template<typename T>
	inline pointer& operator=( T* data ) { this->data = (pvoid)data; return *this; }

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
	inline T& get( uptr offset = 0 ) { return *( (T*)data + offset ); }
	template<typename T>
	inline T const& get( uptr offset = 0 ) const { return *( (T const*)data + offset ); }

private:
	pvoid data;

};

#define mem_align(n) __declspec(align(n))

#define no_vtable __declspec(novtable)

#define aligned(p, n) ((uptr)p % n == 0)

#endif // #ifndef __core_types_h_included_