#include "stdafx.h"
#include "ZLibMgr.h"


//#ifdef _MSC_VER
//#ifdef _DEBUG
//	#pragma comment(lib,"zlibstatD.lib")
//#else
//	#pragma comment(lib,"zlibstat.lib")
//#endif
//#endif

namespace Pdf {
namespace Filter {

ZLib::ZLib()
{
	Reset();
}

void ZLib::Reset()
{
	memset(&m_pZStm,0,sizeof(z_stream));
}

int ZLib::Compress( byte* pbDestination, size_t* pdwDestination, byte* pbSource, size_t cbSource, int iLevel  )
{
	z_stream stream;
	int err;
	stream.next_in = (Bytef*)pbSource;
	stream.avail_in = (uInt)cbSource;
	stream.next_out = pbDestination;
	stream.avail_out = (uInt)*pdwDestination;
	if ( (size_t)stream.avail_out != *pdwDestination)  
		return Z_BUF_ERROR;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, iLevel);//err = deflateInit2(&stream, iLevel, Z_DEFLATED, 15, 8, Z_RLE );
	if (err != Z_OK) 
		return err;

	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) 
	{
		deflateEnd(&stream);
		//return false;//(err == Z_OK);		//
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*pdwDestination = stream.total_out;

	err = deflateEnd(&stream);
//	return err;// == Z_OK);
	//if( err!= Z_OK )
	//	return err;

	return Z_OK;
}
int ZLib::Decompress( byte* pbDestination, size_t* pdwDestination,
				byte* pbSource, size_t cbSource, byte** ppStoppedInSrc )
{
	//z_stream stream;
	int err;
	z_stream* pZStm = &m_pZStm;
	if( !ppStoppedInSrc || pbSource == *ppStoppedInSrc )
	{
		memset(pZStm,0,sizeof(z_stream));
		pZStm->next_in = (Bytef*)pbSource;
		pZStm->avail_in = (uInt)cbSource;

		if ((uLong)pZStm->avail_in != cbSource)// return Z_BUF_ERROR;
			return -7;//Z_BUF_ERROR;
	}

	pZStm->next_out = pbDestination;
	pZStm->avail_out = (uInt)*pdwDestination;
	if ((uLong)pZStm->avail_out != *pdwDestination)// return Z_BUF_ERROR;
		return -7;//Z_BUF_ERROR;

	*pdwDestination = 0;

	if( !ppStoppedInSrc || pbSource == *ppStoppedInSrc )
	{
		pZStm->zalloc = (alloc_func)0;
		pZStm->zfree = (free_func)0;

		err = inflateInit(pZStm);
		if (err != Z_OK)
			return err;
	}

	size_t x = pZStm->total_out;
	while( Z_OK == (err = inflate(pZStm, Z_BLOCK) ))
		;//*pdwDestination += pZStm->total_out;

	if( err == Z_DATA_ERROR )
	{
		pZStm->next_out = pbDestination;
		pZStm->avail_out = (uInt)*pdwDestination;

		int iLoopsBeforeGivingUp = 10;//, iSyncErr = 0;
		while( iLoopsBeforeGivingUp-->0 && (err = inflateSync(pZStm)) != Z_OK && err != Z_DATA_ERROR && err != Z_STREAM_ERROR )
			;
		if( err == Z_OK )
			while( Z_OK == (err = inflate(pZStm, Z_BLOCK) ))
				;//*pdwDestination += pZStm->total_out;
	}

	*pdwDestination = pZStm->total_out-x;

	if( ppStoppedInSrc )
		*ppStoppedInSrc = (byte*)pZStm->next_in;

	if (err != Z_STREAM_END) 
	{
		err = err == Z_OK ? Z_BUF_ERROR : err;
		if( err != Z_BUF_ERROR )
			inflateEnd(pZStm);
		//return false;//(err == Z_OK);		//
		return err;
	}

	//*pdwDestination = pZStm->total_out;

	if( err != Z_BUF_ERROR )
	{
		err = inflateEnd(pZStm);
		memset(pZStm,0,sizeof(z_stream));
	}
	return err;//(err == Z_OK);
}

};};
