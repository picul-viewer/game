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

typedef bind_parameter_type<0> bound_param_0;
typedef bind_parameter_type<1> bound_param_1;
typedef bind_parameter_type<2> bound_param_2;
typedef bind_parameter_type<3> bound_param_3;
typedef bind_parameter_type<4> bound_param_4;
typedef bind_parameter_type<5> bound_param_5;
typedef bind_parameter_type<6> bound_param_6;
typedef bind_parameter_type<7> bound_param_7;
typedef bind_parameter_type<8> bound_param_8;
typedef bind_parameter_type<9> bound_param_9;

static const bound_param_0 bind_param_0;
static const bound_param_1 bind_param_1;
static const bound_param_2 bind_param_2;
static const bound_param_3 bind_param_3;
static const bound_param_4 bind_param_4;
static const bound_param_5 bind_param_5;
static const bound_param_6 bind_param_6;
static const bound_param_7 bind_param_7;
static const bound_param_8 bind_param_8;
static const bound_param_9 bind_param_9;


template<typename FunctionType, typename ... BindingParams>
struct fixed_function;

template<typename FunctionType>
struct auto_function;


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


template<typename FunctionType>
struct function_wrapper;

template<typename Return, typename ... Args>
struct function_wrapper<Return(*)( Args ... )>
{
	typedef Return(*value_type)( Args ... );
	typedef Return return_type;

	inline function_wrapper( )
	{ }

	inline function_wrapper( value_type f ) :
		m_function	( f )
	{ }

	inline Return operator( )( Args ... args ) const
	{
		return m_function( args ... );
	}

	value_type	m_function;
};

template<typename Return, typename This, typename ... Args>
struct function_wrapper<Return(This::*)( Args ... )>
{
	typedef Return(This::*value_type)( Args ... );
	typedef Return return_type;

	inline function_wrapper( )
	{ }

	inline function_wrapper( value_type f ) :
		m_function	( f )
	{ }

	inline Return operator( )( This* this_ptr, Args ... args ) const
	{
		return ( this_ptr->*m_function )( args ... );
	}

	value_type	m_function;
};

template<typename Return, typename This, typename ... Args>
struct function_wrapper<Return(This::*)( Args ... ) const>
{
	typedef Return(This::*value_type)( Args ... ) const;
	typedef Return return_type;

	inline function_wrapper( )
	{ }

	inline function_wrapper( value_type method, This const* this_ptr ) :
		m_function	( method )
	{ }

	inline Return operator( )( This const* this_ptr, Args ... args ) const
	{
		return ( this_ptr->*m_function )( args ... );
	}

	value_type	m_function;
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


template<typename FixedFunctionType>
struct auto_from_fixed_bind_result_helper;

template<typename FunctionType, typename ... Bindings>
struct auto_from_fixed_bind_result_helper<fixed_function<FunctionType, Bindings ...>>
{
	typedef auto_function<typename bind_result<FunctionType>::template params<Bindings ...>::function_type> result_type;
};

}


template<typename FunctionType, typename ... BindingParams>
struct fixed_function
{
protected:
	typedef __lib_function_detail::args_store_type<BindingParams ...> arguments_type;

public:
	inline fixed_function( )
	{ }

	template<typename ... Args>
	inline fixed_function( FunctionType const& functor, Args ... args ) :
		m_function		( functor ),
		m_params_store	( args ... )
	{ }

	template<typename ... Args>
	inline typename __lib_function_detail::function_wrapper<FunctionType>::return_type operator( )( Args ... args )
	{	
		__lib_function_detail::invoke_helper<Args ...>::template inner<
			__lib_function_detail::function_wrapper<FunctionType>,
			typename arguments_type::pair_type>::call( m_function, args ..., m_param_use );
	}

	inline operator bool( )
	{
		return m_function.m_function != nullptr;
	}

protected:
	__lib_function_detail::function_wrapper<FunctionType> m_function;
	union
	{
		arguments_type m_params_store;
		typename arguments_type::pair_type m_param_use;
	};
};


template<typename FunctionReturn, typename ... FunctionArgs>
struct auto_function<FunctionReturn(*)( FunctionArgs ... )>
{
public:
	inline auto_function( )
	{ }

	template<typename FunctionType, typename ... Args>
	inline auto_function( FunctionType const& func, Args ... args ) :
		auto_function( fixed_function<FunctionType, Args ...>( func, args ... ) )
	{ }
	
	template<typename FunctionType, typename ... Args>
	inline auto_function( fixed_function<FunctionType, Args ...> const& func )
	{
		typedef invoke_function_helper<fixed_function<FunctionType, Args ...>> helper;

		m_invoker		= &helper::invoke;
		helper::create	( m_data, func );
	}

	inline FunctionReturn operator( )( FunctionArgs ... args )
	{
		return (FunctionReturn)m_invoker( m_data, args ... );
	}
	
	inline operator bool( )
	{
		return m_invoker != nullptr;
	}

protected:
	typedef FunctionReturn(*invoker_type)( pbyte, FunctionArgs ... );
	
	template<typename FixedFunctionType>
	struct invoke_function_helper;

	template<typename FunctionType, typename ... Bindings>
	struct invoke_function_helper<fixed_function<FunctionType, Bindings ...>>
	{
		typedef FunctionType function_type;
		typedef fixed_function<FunctionType, Bindings ...> fixed_function_type;
		typedef typename __lib_function_detail::template function_wrapper<FunctionType>::return_type return_type;

		static inline void create( pbyte arena, fixed_function_type func )
		{
			ASSERT( sizeof(fixed_function_type) <= ArenaSize, "not enough memory to store all arguments" );

			*(fixed_function_type*)( arena ) = func;
		}

		template<typename ... Args>
		static inline return_type invoke( pbyte arena, FunctionArgs ... args )
		{
			fixed_function_type& func = *(fixed_function_type*)arena;

			return func( args ... );
		}
	};
	
protected:
	enum { Size = 128 };
	enum { ArenaSize = Size - sizeof(invoker_type) };

	invoker_type m_invoker;
	byte m_data[ArenaSize];

};


template<typename FunctionType, typename ... Args>
inline fixed_function<FunctionType, Args ...> fixed_function_bind( FunctionType const& functor, Args ... args )
{
	return fixed_function<FunctionType, Args ...>( functor, args ... );
}

template<typename FunctionType, typename ... Args>
inline auto_function<typename __lib_function_detail::bind_result<FunctionType>::template params<Args ...>::function_type> auto_function_bind( FunctionType const& functor, Args ... args )
{
	typedef auto_function<typename __lib_function_detail::bind_result<FunctionType>::template params<Args ...>::function_type> result_type;

	return result_type( functor, args ... );
}

template<typename FixedFunctionType>
inline typename __lib_function_detail::template auto_from_fixed_bind_result_helper<FixedFunctionType>::result_type auto_function_bind( FixedFunctionType const& functor )
{
	typedef typename __lib_function_detail::template auto_from_fixed_bind_result_helper<FixedFunctionType>::result_type result_type;

	return result_type( functor );
}

#endif // #ifndef __core_function_h_included_