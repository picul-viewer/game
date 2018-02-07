#ifndef __core_function_h_included_
#define __core_function_h_included_

#include <core/types.h>
#include <core/macros.h>

template<uptr Size>
struct mem_align(16) procedure
{
protected:
	using functor_type = void(*)( );

#pragma pack( push, 4 )

	template<typename ... Args>
	struct args_type;
	
	template<>
	struct args_type<>
	{ };
	
	template<typename T>
	struct args_type<T>
	{
		args_type( T left ) :
			left	( left )
		{ }

		T left;

		typedef T first_type;
		typedef args_type<> other_types;
	};

	template<typename T, typename ... Args>
	struct args_type<T, Args ...>
	{
		args_type( T left, Args ... right ) :
			left	( left ),
			right	( right ... )
		{ }

		T left;
		args_type<Args ...> right;

		typedef T first_type;
		typedef args_type<Args ...> other_types;
	};

#pragma pack( pop )


	template<typename ArgsType, typename ... Args>
	struct invoke_helper
	{
		static void call( functor_type functor, ArgsType& args_holder, Args ... args )
		{
			invoke_helper<typename ArgsType::other_types, Args ..., typename ArgsType::first_type>::call(
				functor, args_holder.right, args ..., args_holder.left );
		}
	};
	
	template<typename T, typename ... Args>
	struct invoke_helper<args_type<T>, Args ...>
	{
		static void call( functor_type functor, args_type<T>& args_holder, Args ... args )
		{
			invoke_helper<args_type<>, Args ..., T>::call(
				functor, args_type<>( ), args ..., args_holder.left );
		}
	};

	template<typename ... Args>
	struct invoke_helper<args_type<>, Args ...>
	{
		static void call( functor_type functor, args_type<>& args_holder, Args ... args )
		{
			typedef void(*local_functor_type)( Args ... );

			local_functor_type f	= (local_functor_type)functor;
			f						( args ... );
		}
	};
	
	template<typename ... Args>
	void invoke( )
	{
		 invoke_helper<args_type<Args ...>>::call( functor, *(args_type<Args ...>*)data );
	}

	
	using invoker_type = void(procedure<Size>::*)( );
	
	enum { Args_Size = Size - sizeof(invoker_type) - sizeof(functor_type) };

	invoker_type invoker;
	functor_type functor;
	u8 data[Args_Size];

public:
	inline procedure( )
	{ }

	template<typename ... Args>
	inline procedure( void(*f)( Args ... ), Args ... args )
	{
		ASSERT( sizeof(args_type<Args ...>) <= Args_Size );
		
		invoker							= &procedure<Size>::invoke<Args ...>;
		functor							= (functor_type)f;
		new ( data ) args_type<Args ...>( args ... );
	}

	void operator( )( )
	{
		( this->*invoker )( );
	}
};

template<>
struct mem_align(8) procedure<8>
{
protected:
	using functor_type = void(*)( );

	functor_type functor;

public:
	inline procedure( )
	{ }

	inline procedure( void(*f)( ) )
	{
		functor = f;
	}

	void operator( )( )
	{
		functor( );
	}
};

#endif // #ifndef __core_function_h_included_