#ifndef __LOG_DEFINE_H__
#define __LOG_DEFINE_H__

#define PRINT_MACRO_HELPER(x)  #x
#define PRINT_MACRO(x)         #x "=" PRINT_MACRO_HELPER(x)

#if defined WIN32 || defined _WINDLL || defined __CYGWIN__
	#if defined DREAMSKY_EXPORTS
		#if defined __GNUC__ || defined __clang__
			#pragma message("log exports in gnuc-win.")
			#define LOG_API           __attribute__ ((dllexport))
		#elif defined _MSC_VER
			#pragma message("log exports in msvc-win.")
			#define LOG_API           __declspec(dllexport)
		#else
			#define LOG_API
		#endif
	#else
		#if defined                        __GNUC__ || defined __clang__
			#define LOG_API           __attribute__ ((dllimport))
		#elif defined _MSC_VER
			#define LOG_API           __declspec(dllimport)
		#else
			#define LOG_API
		#endif
	#endif
#else
	#if defined DREAMSKY_EXPORTS
		#if                                __GNUC__ >= 4 || defined __clang__
			#pragma message("log exports in gnuc-unix.")
			#define LOG_API           __attribute__((visibility ("default")))
		#else
			#define LOG_API
		#endif
	#else
		#define LOG_API
	#endif
#endif

#endif //!__LOG_DEFINE_H__