#pragma once

#include "ExportDef.h"

#ifdef __GNUC__

	#ifndef interface
	#define interface struct
	#endif

#endif

namespace Pdf
{
	typedef long long PDF_API int64;

	//typedef PDF_API unsigned char byte;
	typedef unsigned char PDF_API byte;

};// namespace Pdf
