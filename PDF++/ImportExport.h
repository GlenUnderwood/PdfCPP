#ifndef PDF_API

	#ifdef _USRDLL

		#if defined _WIN32 || defined __CYGWIN__ || _WINDOWS
			#ifdef __GNUC__
			  #define PDF_API __attribute__ ((dllimport))
			#else
			  #define PDF_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
			#endif
		  #define DLL_LOCAL
		#else
		  #if __GNUC__ >= 4
			#define PDF_API __attribute__ ((visibility ("default")))
			#define PDF_API_LOCAL  __attribute__ ((visibility ("hidden")))
		  #else
			#define PDF_API
			#define PDF_API_LOCAL
		  #endif
		#endif

		#ifndef _UNICODE
			#ifdef _DEBUG
				#pragma comment(lib, "PDF++AD.lib" )
			#else
				#pragma comment(lib, "PDF++A.lib" )
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "PDF++D.lib" )
			#else
				#pragma comment(lib, "PDF++.lib" )
			#endif
		#endif
	#else

		// if this isn't a DLL then blank out PDF_API so no __declspec()s take place.
		#define PDF_API 

		#ifndef _UNICODE
			#ifdef _DEBUG
				#pragma comment(lib, "PDF++LibSAD.lib" )
			#else
				#pragma comment(lib, "PDF++LibSA.lib" )
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "PDF++LibSD.lib" )
			#else
				#pragma comment(lib, "PDF++LibS.lib" )
			#endif
		#endif
	#endif
#endif

