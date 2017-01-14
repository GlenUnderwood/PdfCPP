#pragma once

#ifndef PDF_API

//#ifdef PDF_EXPORTS
//#define PDF_API __declspec(dllexport)
//#else
//#define PDF_API __declspec(dllimport)
//#endif
#if defined _WIN32 || defined __CYGWIN__ || _WINDOWS
  #ifdef PDF_EXPORTS
    #ifdef __GNUC__
      #define PDF_API __attribute__ ((dllexport))
    #else
      #define PDF_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define PDF_API __attribute__ ((dllimport))
    #else
      #define PDF_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
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


#endif
