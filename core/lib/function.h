#ifndef __core_function_h_included_
#define __core_function_h_included_

#include <types.h>
#include <macros.h>

#include <xmemory>

template<u32 index>
struct bind_parameter_type
{
	enum { value = index };
};

static const bind_parameter_type<0> bind_param_0;
static const bind_parameter_type<1> bind_param_1;
static const bind_parameter_type<2> bind_param_2;
static const bind_parameter_type<3> bind_param_3;
static const bind_parameter_type<4> bind_param_4;
static const bind_parameter_type<5> bind_param_5;
static const bind_parameter_type<6> bind_param_6;
static const bind_parameter_type<7> bind_param_7;
static const bind_parameter_type<8> bind_param_8;
static const bind_parameter_type<9> bind_param_9;


namespace __lib_function_detail
{

template<typename ... ArgPair>
struct arg_pair_type;

template<>
struct arg_pair_type<>
{
	typedef void left_type;
	typedef void right_type;
};

template<typename T>
struct arg_pair_type<T>
{
	typedef T left_type;
	typedef arg_pair_type<> right_type;

	left_type left;
};

template<u32 Index>
struct arg_pair_type<bind_parameter_type<Index>>
{
	typedef bind_parameter_type<Index> left_type;
	typedef arg_pair_type<> right_type;
};

template<typename L, typename R>
struct arg_pair_type<L, R>
{
	typedef L left_type;
	typedef R right_type;

	left_type left;
	right_type right;
};

template<u32 Index, typename R>
struct arg_pair_type<bind_parameter_type<Index>, R>
{
	typedef bind_parameter_type<Index> left_type;
	typedef R right_type;

	right_type right;
};


template<typename ... Args>
struct args_store_type;

template<>
struct args_store_type<>
{
	typedef arg_pair_type<> pair_type;

	args_store_type( ) = default;

	operator pair_type( )
	{
		return *(pair_type*)this;
	}
};

template<typename T>
struct args_store_type<T>
{
	typedef arg_pair_type<T> pair_type;

	args_store_type( T left ) :
		left	( left )
	{ }

	operator pair_type( )
	{
		return *(pair_type*)this;
	}

	T left;
};

template<u32 Index>
struct args_store_type<bind_parameter_type<Index>>
{
	typedef arg_pair_type<bind_parameter_type<Index>> pair_type;
	
	args_store_type( )
	{ }

	args_store_type( bind_parameter_type<Index> )
	{ }

	operator pair_type( )
	{
		return *(pair_type*)this;
	}
};

template<typename T, typename ... Args>
struct args_store_type<T, Args ...>
{
	typedef arg_pair_type<T, typename args_store_type<Args ...>::pair_type> pair_type;

	args_store_type( T left, Args ... right ) :
		left	( left ),
		right	( right ... )
	{ }

	operator pair_type( )
	{
		return *(pair_type*)this;
	}

	T left;
	args_store_type<Args ...> right;
};

template<u32 Index, typename ... Args>
struct args_store_type<bind_parameter_type<Index>, Args ...>
{
	typedef arg_pair_type<bind_parameter_type<Index>, typename args_store_type<Args ...>::pair_type> pair_type;
	
	args_store_type( Args ... right ) :
		right	( right ... )
	{ }

	args_store_type( bind_parameter_type<Index>, Args ... right ) :
		right	( right ... )
	{ }

	operator pair_type( )
	{
		return *(pair_type*)this;
	}

	args_store_type<Args ...> right;
};


template<u32 Index, typename ... Args>
struct type_by_index;

template<u32 Index, typename T, typename ... Args>
struct type_by_index<Index, T, Args ...>
{
	typedef typename type_by_index<Index - 1, Args ...>::value_type value_type;
};
	
template<typename T, typename ... Args>
struct type_by_index<0, T, Args ...>
{
	typedef T value_type;
};
	
template<u32 Index, typename T>
struct type_by_index<Index, T>
{
	static_assert( Index == 0, "bad parameter index" );

	typedef T value_type;
};
	

template<typename Return, u32 Index, typename ... Args>
struct value_by_index_helper;

template<typename Return, u32 Index, typename T, typename ... Args>
struct value_by_index_helper<Return, Index, T, Args ...>
{
	static inline Return get( T left, Args ... right )
	{
		return value_by_index_helper<Return, Index - 1, Args ...>::get( right ... );
	}
};
	
template<typename Return, typename T, typename ... Args>
struct value_by_index_helper<Return, 0, T, Args ...>
{
	static inline Return get( T left, Args ... right )
	{
		return left;
	}
};
	
template<typename Return, u32 Index, typename T>
struct value_by_index_helper<Return, Index, T>
{
	static_assert( Index == 0, "bad parameter index" );

	static inline Return get( T left )
	{
		return left;
	}
};
	
template<u32 Index, typename ... Args>
static inline typename type_by_index<Index, Args ...>::value_type value_by_index( Args ... args )
{
	return value_by_index_helper<typename type_by_index<Index, Args ...>::value_type, Index, Args ...>::get( args ... );
}


template<typename ... CallerArgs>
struct invoke_helper
{
	template<typename FunctorType, typename ArgsType, typename ... Args>
	struct inner;
	
	template<typename FunctorType, typename LeftType, typename RightArgsType, typename ... Args>
	struct inner<FunctorType, arg_pair_type<LeftType, RightArgsType>, Args ...>
	{
		static inline typename FunctorType::return_type call( FunctorType const& functor, CallerArgs ... caller_args, arg_pair_type<LeftType, RightArgsType>& args_holder, Args ... args )
		{
			return invoke_helper<CallerArgs ...>::template inner<FunctorType, RightArgsType, Args ..., LeftType>::call(
				functor, caller_args ..., args_holder.right, args ..., args_holder.left );
		}
	};
	
	template<typename FunctorType, u32 BindIndex, typename RightArgsType, typename ... Args>
	struct inner<FunctorType, arg_pair_type<bind_parameter_type<BindIndex>, RightArgsType>, Args ...>
	{
		static inline typename FunctorType::return_type call( FunctorType const& functor, CallerArgs ... caller_args, arg_pair_type<bind_parameter_type<BindIndex>, RightArgsType>& args_holder, Args ... args )
		{
			return invoke_helper<CallerArgs ...>::template inner<FunctorType, RightArgsType, Args ..., typename type_by_index<BindIndex, CallerArgs ...>::value_type>::call(
				functor, caller_args ..., args_holder.right, args ..., value_by_index<BindIndex, CallerArgs ...>( caller_args ... ) );
		}
	};
	
	template<typename FunctorType, typename T, typename ... Args>
	struct inner<FunctorType, arg_pair_type<T>, Args ...>
	{
		static inline typename FunctorType::return_type call( FunctorType const& functor, CallerArgs ... caller_args, arg_pair_type<T>& args_holder, Args ... args )
		{
			return invoke_helper<CallerArgs ...>::template inner<FunctorType, arg_pair_type<>, Args ..., T>::call(
				functor, caller_args ..., arg_pair_type<>( ), args ..., args_holder.left );
		}
	};
	
	template<typename FunctorType, u32 BindIndex, typename ... Args>
	struct inner<FunctorType, arg_pair_type<bind_parameter_type<BindIndex>>, Args ...>
	{
		static inline typename FunctorType::return_type call( FunctorType const& functor, CallerArgs ... caller_args, arg_pair_type<bind_parameter_type<BindIndex>>& args_holder, Args ... args )
		{
			return invoke_helper<CallerArgs ...>::template inner<FunctorType, arg_pair_type<>, Args ..., typename type_by_index<BindIndex, CallerArgs ...>::value_type>::call(
				functor, caller_args ..., arg_pair_type<>( ), args ..., value_by_index<BindIndex, CallerArgs ...>( caller_args ... ) );
		}
	};

	template<typename FunctorType, typename ... Args>
	struct inner<FunctorType, arg_pair_type<>, Args ...>
	{
		static inline typename FunctorType::return_type call( FunctorType const& functor, CallerArgs ... function_args, arg_pair_type<>& args_holder, Args ... args )
		{
			return functor( args ... );
		}
	};
};



struct dummy_type;

template<typename Type, u32 Index, typename ResultReturn, typename ... ResultArgs>
struct set_binding_param;

template<typename Type, u32 Index, typename ResultReturn, typename ... ResultArgs>
struct set_binding_param
{
	template<typename ... Args>
	struct inner;

	template<typename T, typename ... Args>
	struct inner<T, Args ...>
	{
		typedef typename set_binding_param<Type, Index - 1, ResultReturn, ResultArgs ..., T>::template inner<Args ...>::function_type function_type;
	};
	
	template<>
	struct inner<>
	{
		typedef typename set_binding_param<Type, Index - 1, ResultReturn, ResultArgs ..., dummy_type>::template inner<>::function_type function_type;
	};
};

template<typename Type, typename ResultReturn, typename ... ResultArgs>
struct set_binding_param<Type, 0, ResultReturn, ResultArgs ...>
{
	template<typename ... Args>
	struct inner;

	template<typename ... Args>
	struct inner<dummy_type, Args ...>
	{
		typedef ResultReturn(*function_type)( ResultArgs ..., Type, Args ... );
	};
	
	template<>
	struct inner<>
	{
		typedef ResultReturn(*function_type)( ResultArgs ..., Type );
	};
};


template<typename Functor>
struct bind_resolve;

template<typename ResultReturn, typename ... ResultArgs>
struct bind_resolve<ResultReturn(*)( ResultArgs ... )>
{
	template<typename ... Args>
	struct args;

	template<>
	struct args<>
	{
		template<typename ... Params>
		struct params;

		template<>
		struct params<>
		{
			typedef ResultReturn (*function_type)( ResultArgs ... );
		};
	};

	template<typename A, typename ... Args>
	struct args<A, Args ...>
	{
		template<typename ... Params>
		struct params;

		template<typename ... Params>
		struct params<A, Params ...>
		{
			typedef typename bind_resolve<ResultReturn(*)( ResultArgs ... )>::template args<Args ...>::template params<Params ...>::function_type function_type;
		};

		template<u32 Index, typename ... Params>
		struct params<bind_parameter_type<Index>, Params ...>
		{
			typedef typename bind_resolve<typename set_binding_param<A, Index, ResultReturn>::template inner<ResultArgs ...>::function_type>::template args<Args ...>::template params<Params ...>::function_type function_type;
		};
	};

	template<typename A>
	struct args<A>
	{
		template<typename ... Params>
		struct params;

		template<>
		struct params<A>
		{
			typedef ResultReturn (*function_type)( ResultArgs ... );
		};

		template<u32 Index, typename ... Params>
		struct params<bind_parameter_type<Index>, Params ...>
		{
			typedef typename set_binding_param<A, Index, ResultReturn>::template inner<ResultArgs ...>::function_type function_type;
		};
	};
};

template<typename Functor>
struct bind_result;

template<typename ResultReturn, typename ... ResultArgs>
struct bind_result<ResultReturn(*)( ResultArgs ... )>
{
	template<typename ... Params>
	struct params
	{
		typedef typename bind_resolve<ResultReturn(*)( )>::template args<ResultArgs ...>::template params<Params ...>::function_type function_type;
	};
};

template<typename ResultReturn, typename ResultThis, typename ... ResultArgs>
struct bind_result<ResultReturn(ResultThis::*)( ResultArgs ... )>
{
	template<typename ... Params>
	struct params;

	template<typename ... Params>
	struct params<ResultThis*, Params ...>
	{
		typedef typename bind_resolve<ResultReturn(*)( )>::template args<ResultArgs ...>::template params<Params ...>::function_type function_type;
	};
};

template<typename ResultReturn, typename ResultThis, typename ... ResultArgs>
struct bind_result<ResultReturn(ResultThis::*)( ResultArgs ... ) const>
{
	template<typename ... Params>
	struct params;

	template<typename ... Params>
	struct params<ResultThis const*, Params ...>
	{
		typedef typename bind_resolve<ResultReturn(*)( )>::template args<ResultArgs ...>::template params<Params ...>::function_type function_type;
	};
};

}


template<typename FunctionType>
struct function;

template<typename FunctionReturn, typename ... FunctionArgs>
struct function<FunctionReturn(*)( FunctionArgs ... )>
{
public:
	template<typename FunctorType, typename ... Args>
	inline function( FunctorType const& func, Args ... args )
	{
		typedef invoke_function_helper<FunctorType> helper;

		invoker			= &helper::invoke<Args ...>;
		helper::create	( data, func, args ... );
	}

	FunctionReturn operator( )( FunctionArgs ... args )
	{
		return (FunctionReturn)invoker( data, args ... );
	}

protected:
	typedef FunctionReturn(*invoker_type)( pbyte, FunctionArgs ... );
		
	template<typename FunctorType>
	struct invoke_function_helper;

	template<typename Return, typename ... FArgs>
	struct invoke_function_helper<Return(*)( FArgs ... )>
	{
		typedef Return(*function_type)( FArgs ... );

		struct bind
		{
		public:
			typedef Return(*value_type)( FArgs ... );
			typedef Return return_type;

			inline bind( value_type method ) :
				m_function	( method )
			{ }

			inline Return operator( )( FArgs ... args ) const
			{
				return m_function( args ... );
			}

		protected:
			value_type	m_function;
		};
		
		template<typename ... Args>
		static inline void create( pbyte arena, function_type func, Args ... args )
		{
			ASSERT( sizeof(function_type) + sizeof(__lib_function_detail::args_store_type<Args ...>) <= ArenaSize, "not enough memory to store all arguments" );

			*(function_type*)( arena ) = func;
			new ( arena + sizeof(function_type) ) __lib_function_detail::args_store_type<Args ...>( args ... );
		}

		template<typename ... Args>
		static inline Return invoke( pbyte arena, FunctionArgs ... args )
		{
			typedef typename __lib_function_detail::args_store_type<Args ...>::pair_type params_type;

			function_type const&	func_value = *(function_type*)( arena );
			params_type&			args_value = *(params_type*)( arena + sizeof(function_type) );

			return __lib_function_detail::invoke_helper<FunctionArgs ...>::template inner<bind, params_type>::call( bind( func_value ), args ..., args_value );
		}
	};
	
	template<typename Return, typename This, typename ... FArgs>
	struct invoke_function_helper<Return(This::*)( FArgs ... )>
	{
		typedef Return(This::*function_type)( FArgs ... );

		struct bind
		{
		public:
			typedef Return(This::*value_type)( FArgs ... );
			typedef Return return_type;

			inline bind( value_type method, This* this_ptr ) :
				m_method	( method ),
				m_this_ptr	( this_ptr )
			{ }

			inline Return operator( )( FArgs ... args ) const
			{
				return ( m_this_ptr->*m_method )( args ... );
			}

		protected:
			value_type	m_method;
			This*		m_this_ptr;
		};
		
		template<typename ... Args>
		static inline void create( pbyte arena, function_type func, This* this_ptr, Args ... args )
		{
			ASSERT( sizeof(function_type) + sizeof(This*) + sizeof(__lib_function_detail::args_store_type<Args ...>) <= ArenaSize, "not enough memory to store all arguments" );

			*(function_type*)( arena ) = func;
			*(This**)( arena + sizeof(function_type) ) = this_ptr;
			new ( arena + sizeof(function_type) + sizeof(This*) ) __lib_function_detail::args_store_type<Args ...>( args ... );
		}
		
		template<typename ... Args>
		static inline Return invoke( pbyte arena, FunctionArgs ... args )
		{
			typedef typename args_store_type<Args ...>::pair_type params_type;

			function_type	func_value = *(function_type*)( arena );
			This*			this_value = *(This**)( arena + sizeof(function_type) );
			params_type&	args_value = *(params_type*)( arena + sizeof(function_type) + sizeof(This*) );
			
			return __lib_function_detail::invoke_helper<FunctionArgs ...>::template inner<bind, params_type>::call( bind( func_value, this_value ), args ..., args_value );
		}
	};
	
	template<typename Return, typename This, typename ... FArgs>
	struct invoke_function_helper<Return(This::*)( FArgs ... ) const>
	{
		typedef Return(This::*function_type)( FArgs ... ) const;

		struct bind
		{
		public:
			typedef Return(This::*value_type)( FArgs ... ) const;
			typedef Return return_type;

			inline bind( value_type method, This const* this_ptr ) :
				m_method	( method ),
				m_this_ptr	( this_ptr )
			{ }

			inline Return operator( )( FArgs ... args ) const
			{
				return ( m_this_ptr->*m_method )( args ... );
			}

		protected:
			value_type	m_method;
			This const*	m_this_ptr;
		};
		
		template<typename ... Args>
		static inline void create( pbyte arena, function_type func, This* this_ptr, Args ... args )
		{
			ASSERT( sizeof(function_type) + sizeof(This const*) + sizeof(__lib_function_detail::args_store_type<Args ...>) <= ArenaSize, "not enough memory to store all arguments" );

			*(function_type*)( arena ) = func;
			*(This const**)( arena + sizeof(function_type) ) = this_ptr;
			new ( arena + sizeof(function_type) + sizeof(This const*) ) __lib_function_detail::args_store_type<Args ...>( args ... );
		}
		
		template<typename ... Args>
		static inline Return invoke( pbyte arena, Args ... args )
		{
			typedef typename args_store_type<Args ...>::pair_type params_type;

			function_type	func_value = *(function_type*)( arena );
			This const*		this_value = *(This const**)( arena + sizeof(function_type) );
			params_type&	args_value = *(params_type*)( arena + sizeof(function_type) + sizeof(This const*) );
			
			return __lib_function_detail::invoke_helper<FunctionArgs ...>::template inner<bind, params_type>::call( bind( func_value, this_value ), args ..., args_value );
		}
	};
	
protected:
	enum { Size = 128 };
	enum { ArenaSize = Size - sizeof(invoker_type) };

	invoker_type invoker;
	byte data[ArenaSize];

};


template<typename Functor, typename ... Args>
function<typename __lib_function_detail::bind_result<Functor>::template params<Args ...>::function_type> function_bind( Functor const& functor, Args ... args )
{
	typedef function<typename __lib_function_detail::bind_result<Functor>::template params<Args ...>::function_type> result_type;

	return result_type( functor, args ... );
}

#endif // #ifndef __core_function_h_included_