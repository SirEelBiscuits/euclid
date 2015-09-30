//if this doesn't work, fuck your compiler
#pragma once

// Flags used for platform/version stuff:

#ifdef _DEBUG
#	define EUCLID_DEBUG
#endif

//Set of macros to suppress bad warnings from /Wall
#ifdef _MSC_VER
#	define PREAMBLE \
	__pragma(once) \
	__pragma(warning(disable: 4191 4512 4514 4625 4626 4661 4668 4710 4820 4711 4996 4814))

#	define PRE_STD_LIB\
	__pragma(warning(push))\
	__pragma(warning(disable: 4266 4350 4365 4471 4625 4626 4640 4668 4917 4986 4987 4265 4571 4062 4355))

#	define POST_STD_LIB \
	__pragma(warning(pop))

#	define REMOVE_NUMBER_CONVERSION_WARNINGS \
	__pragma(warning(disable: 4244 4365))

# define REMOVE_VS_WARNING(id) \
	__pragma(warning(disable: id))

#	define ALLOW_VS_WARNING(id) \
	__pragma(warning(default: id))

#	ifdef _DEBUG
#		define DEBUG_BREAK() __debugbreak()
#	else
#		define DEBUG_BREAK()
#	endif

#else
#	define PREAMBLE _Pragma("once")
#	define PRE_STD_LIB
#	define POST_STD_LIB
#	define REMOVE_NUMBER_CONVERSION_WARNINGS
#	define REMOVE_VS_WARNING(id)
#	define ALLOW_VS_WARNING(id)
#	ifdef EUCLID_DEBUG
#		define DEBUG_BREAK() __asm__ ("int 3")
#	else
#		define DEBUG_BREAK()
#	endif
#endif

#ifdef EUCLID_DEBUG
#	define ASSERT(p, printfargs...) \
	do {\
 		if(!p) {\
		 	printf(printfargs);\
		 	DEBUG_BREAK();\
		}\
	} while(false)
#	define CRITICAL_ASSERT(p, printfargs...) ASSERT(p, printfargs)
#else
#	define ASSERT(p, printfargs...)
# define CRITICAL_ASSERT(p, printfargs...) \
	do {\
		if(!p) {\
			printf(printfargs);\
			exit(EXIT_FAILURE);\
		}\
	} while(false)
#endif

#define PORTABLE_PATH_MAX 260
