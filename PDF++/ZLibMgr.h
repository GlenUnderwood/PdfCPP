#pragma once

// for 1.2.8 to work right
#define ZLIB_WINAPI

#include <zlib.h>

namespace Pdf {
namespace Filter {

class ZLib// : public ICompressor
{
public:
/*
	public enum zlibErr : int
	{
		Z_OK = 0,
		Z_STREAM_END   = 1,
		Z_NEED_DICT    = 2,
		Z_ERRNO        =(-1),
		Z_STREAM_ERROR =(-2),
		Z_DATA_ERROR   =(-3),
		Z_MEM_ERROR    =(-4),
		Z_BUF_ERROR    =(-5),
		Z_VERSION_ERROR =(-6)
	};
*/	/*enum
	{
		levNone					= 0,
		levBestSpeed			= 1,
		levBestCompression		= 9,
		levDefaultCompression	= -1
	} CompressionLevel;*/

	ZLib();

	void Reset();

	int Compress( byte* pbDestination, size_t* pdwDestination, byte* pbSource, size_t cbSource, int iLevel = -1);

	int Decompress( byte* pbDestination, size_t* pdwDestination,
					byte* pbSource, size_t cbSource, byte** ppStoppedInSrc );

private:
	z_stream m_pZStm;
};

};
};