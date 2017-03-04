#pragma once

#ifndef PDF_API

#if defined(WINAPI_FAMILY)
#	if WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
#		pragma message("This is a Windows Store App")
#	elif WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#		pragma message("This is a Windows Desktop App")
#	else 
#		error Unhandled win app family type
#	endif
#endif

#if defined(_WIN32) || defined(__CYGWIN__) || defined(_WINDOWS)
#	ifdef _USRDLL
#		ifdef PDF_EXPORTS
#			ifdef __GNUC__
#				define PDF_API __attribute__ ((dllexport))
#			else
#				define PDF_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#			endif
#		else
#			ifdef __GNUC__
#				define PDF_API __attribute__ ((dllimport))
#			else
#				define PDF_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#			endif
#		endif
#		define DLL_LOCAL
#	else // #ifdef _USRDLL
#		define PDF_API
#		define PDF_API_LOCAL
#	endif
#else
#	if __GNUC__ >= 4
#		define PDF_API __attribute__ ((visibility ("default")))
#		define PDF_API_LOCAL  __attribute__ ((visibility ("hidden")))
#	else
#		define PDF_API
#		define PDF_API_LOCAL
#	endif
#endif


#endif
