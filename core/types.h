#ifndef __core_types_h_included_
#define __core_types_h_included_

#include <stdio.h>

#include <array>
#include <random>
#include <windows.h>

#undef min
#undef max

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

class rand32
{
protected:
	u64 n;
public:
	inline rand32( ) : n( 1 ) {}
	inline rand32( u32 seed ) : n( seed ) {}
	inline u32 rand( )
	{
		n = 6364136223846793005 * n + 1442695040888963407;
		return n >> 32;
	}
	inline u32 rand( u32 max )
	{
		return rand( ) % max;
	}
	inline u32 rand( u32 a, u32 b )
	{
		return a + rand( ) % ( b - a + 1 );
	}
	inline f32 randf( )
	{
		return rand( ) / (f32)UINT_MAX;
	}
	inline f32 randf( f32 max )
	{
		return randf( ) * max;
	}
	inline f32 randf( f32 a, f32 b )
	{
		return a + randf( ) * ( b - a );
	}
};

template<typename T, typename ... CreateArgs>
struct link_counter
{
	T data;
	size_t refCnt = 0;

	inline void create( CreateArgs ... args )
	{
		if ( refCnt == 0 )
			data.create( args ... );
		++refCnt;
	}

	inline void destroy( )
	{
		--refCnt;
		if ( refCnt == 0 )
			data.destroy( );
	}

	inline T* operator->( ) { return &data; }

	inline T& operator*( ) { return data; }
	inline T const& operator*( ) const { return data; }
};

enum
{
	Kb	= 1024,
	Mb	= 1024 * Kb,
	Gb	= 1024 * Mb
};

template<uptr size>
struct memory_helper
{
	u8 data[size];
};

template<uptr size, typename T>
struct typed_memory_helper
{
	u8 data[size];

	operator T&( ) { return *(T*)data; }
	operator T const&( ) const { return *(T*)data; }
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
	inline pointer( ) { }
	inline pointer( pvoid data ) { this->data = data; }

	template<typename T>
	inline operator T*( ) { return (T*)data; }
	
	inline operator bool( ) { return data != nullptr; }
	
	template<typename T>
	inline pointer& operator=( T* data ) { this->data = (pvoid)data; return *this; }

	inline pointer& operator=( nullptr_t data ) { this->data = (pvoid)data; return *this; }

	inline bool operator==( pointer p ) const { return data == p.data; }
	inline bool operator!=( pointer p ) const { return data != p.data; }
	inline bool operator>( pointer p ) const { return data > p.data; }
	inline bool operator<( pointer p ) const { return data < p.data; }
	inline bool operator>=( pointer p ) const { return data >= p.data; }
	inline bool operator<=( pointer p ) const { return data <= p.data; }

	inline pointer operator+( uptr n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( uptr n ) const { return pointer( (pbyte)data - n ); }
	inline pointer operator+( ptr n ) const { return pointer( (pbyte)data + n ); }
	inline pointer operator-( ptr n ) const { return pointer( (pbyte)data - n ); }
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
	inline pointer& operator--( ) { data = (pbyte)data + 1; return *this; }

	inline pointer operator++( int n ) { pointer p( *this ); data = (pbyte)data + 1; return p; }
	inline pointer operator--( int n ) { pointer p( *this ); data = (pbyte)data + 1; return p; }

	template<typename T>
	inline T& get( ) { return *(T*)data; }
	template<typename T>
	inline T const& get( ) const { return *(T const*)data; }

protected:
	pvoid data;
};

#endif // #ifndef __core_types_h_included_