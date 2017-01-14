// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WINDOWS
	//_MSC_VER

	#define NOMINMAX

	#include "targetver.h"

	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#include <windows.h>

	#include <stdio.h>
	#include <io.h>
	#include <memory>


	#pragma warning( disable:4482 )
	#pragma warning( disable:4251 )
	#pragma warning( disable:4146 )
	#pragma warning( disable:4244 )

#else // Unix
// __GNUC__
	#include <inttypes.h>
	#include <stdio.h>
	#include <memory>
	#include <cstring>
	#include <string.h>

	/*#ifndef __int64
	typedef long long __int64;
	#endif*/
	#ifndef interface
	#define interface struct
	#endif
	/*#ifndef _stricmp
	#define _stricmp strcasecmp
	#endif*/
#endif


// reference additional headers your program requires here

#include <fstream>
#include <exception>
#include <memory>
#include <algorithm>
#include <iterator>

#include "pdfTypes.h"
#include "streams.h"
#include "pdfException.h"

#define DWORDAlignedBytes(w, bpp)\
			((((w * bpp) + 31) & ~31) >> 3)

using namespace Pdf::Exception;

using namespace std;

namespace Pdf
{
namespace Math
{
	template< typename T>
	T Min( T a, T b )
	{
		return a < b ? a:b;
	}
	template< typename T>
	T Max( T a, T b )
	{
		return a > b ? a:b;
	}
}
}
