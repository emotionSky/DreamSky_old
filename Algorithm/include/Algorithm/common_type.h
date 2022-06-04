#ifndef __COMMON_TYPE_H__
#define __COMMON_TYPE_H__

#define PRINT_MACRO_HELPER(x)  #x
#define PRINT_MACRO(x)         #x"="PRINT_MACRO_HELPER(x)

#if defined WIN32 || defined _WINDLL || defined __CYGWIN__
	#if defined ALGORITHM_EXPORTS
		#if defined __GNUC__
			#pragma message("log exports in gnuc-win.")
			#define DREAMSKY_API __attribute__ ((dllexport))
		#elif defined(_MSC_VER)
			#pragma message("log exports in msvc-win.")
			#define DREAMSKY_API __declspec(dllexport)
		#else
			#define DREAMSKY_API
		#endif
	#else
		#if defined __GNUC__
			#define DREAMSKY_API __attribute__ ((dllimport))
		#elif defined(_MSC_VER)
			#define DREAMSKY_API  __declspec(dllimport)
		#else
			#define DREAMSKY_API
		#endif
	#endif
#else
	#if defined LOG_EXPORTS
		#if __GNUC__ >= 4
			#pragma message("log exports in gnuc-unix.")
			#define DREAMSKY_API	__attribute__((visibility ("default")))
		#else
			#define DREAMSKY_API
		#endif
	#else
		#define DREAMSKY_API
	#endif
#endif

#endif