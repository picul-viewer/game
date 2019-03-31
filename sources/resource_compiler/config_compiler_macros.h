#ifndef GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_MACROS_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_MACROS_H_INCLUDED

#define DECLARE_TYPE( type )																	\
	bool compile_config_##type( pvoid const data,												\
								uptr const size,												\
								pcstr const file_name,											\
								pcstr const output_path );										\

#define COMPILE_TYPE( type )																	\
	if ( config_type == #type )																	\
	{																							\
		result = compile_config_##type( data, size, file_name.c_str( ), output_path.c_str( ) );	\
		found_type = true;																		\
	}																							\

#define DECLARE( types ) types( DECLARE_TYPE )

#define COMPILE( types ) types( COMPILE_TYPE )

#endif // #ifndef GUARD_RESOURCE_COMPILER_CONFIG_COMPILER_MACROS_H_INCLUDED