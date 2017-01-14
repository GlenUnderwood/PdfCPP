#include "stdio.h"
#include "stdafx.h"
#include "TiffMgr.h"
#include <sstream>

#ifdef _WINDOWS
#define USE_WIN32_FILEIO
#define __WIN32__
#endif
#include <tiffio.h>
#include <tiffio.hxx>

namespace Pdf {
namespace Filter {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class CEnDecodeProcs
//{
//public:
//	CEnDecodeProcs( CSharedBuff& buffDest ) : m_buffDest(buffDest), m_dwOffset(0)
//	{};
//
//	CSharedBuff& m_buffDest;
//
//	//Encoding ----------------------------------
//	// Memory block used for doing in-memory G4 compression via the tiff library.
//	//HANDLE	g_hMem = NULL;
//	size_t	m_dwOffset;
//
//	// Some callbacks we need because we are doing our stuff in-memory
//	/*static*/
//	tsize_t	CCITT_ReadProc (thandle_t fd, tdata_t buf, tsize_t size)
//	{
//		return 0;
//	}
//	/*static*/
//	tsize_t	CCITT_WriteProc (thandle_t fd, tdata_t buf, tsize_t size)
//	{
//		// This bit isn't needed since they are the TIFF headers, and all we want is the actual G4 block.
//		if ( (size == 8) && (((char *) buf)[0] == 'I') && (((char *) buf)[1] == 'I')
//				&& (((char *) buf)[2] == 42))
//		{
//				// Skip the header -- little endian
//		}
//		else if (  (size == 8) && (((char *) buf)[0] == 'M') &&
//				(((char *) buf)[1] == 'M') && (((char *) buf)[2] == 42))
//		{
//				// Skip the header -- big endian
//		}
//		else 
//		{
//			// Allocate some memory if we haven't already 
//			if (m_buffDest.GetLength() == 0 )
//			{
//				try{
//				m_buffDest.Resize(size,false);
//				}
//				catch(...){}
//
//				if (m_buffDest.GetLength() != size )
//				{
//					m_buffDest.Clear();
//					m_dwOffset = 0;
//
//					return 0;//exit(42);
//				}
//			}
//			else // and if we did, re-alloc
//			{
//				try{
//				m_buffDest.Resize(size + m_dwOffset, true);
//				}
//				catch(...){}
//				if (m_buffDest.GetLength() != size + m_dwOffset )
//				{
//					m_buffDest.Clear();
//					m_dwOffset = 0;
//
//					//fprintf(stderr, "Could not grow the tiff conversion memory buffer\n");
//					return 0;//exit(42);
//				}
//			}
//
//			// Now move the image data into the buffer
//			memcpy( &(m_buffDest.get())[ m_dwOffset ], buf, size);
//			m_dwOffset += size;
//		}
//
//		return (size);
//	}
//	// these funcs do nothing because we don't need to do anything for the memory block but the library still insists that they exist.
//	/*static*/ toff_t	CCITT_SeekProc (thandle_t fd, toff_t off, int i)
//	{
//		return off;
//	}
//	/*static*/ int		CCITT_CloseProc (thandle_t fd)
//	{
//		return 0;
//	}
//	/*static*/ toff_t	CCITT_SizeProc (thandle_t fd)
//	{
//		return 0;
//	}
//	/*static*/ int		CCITT_MapFileProc (thandle_t fd, tdata_t* ppBuf, toff_t* pOff)
//	{
//		return 0;
//	}
//	/*static*/ void		CCITT_UnmapFileProc (thandle_t fd, tdata_t pBuf, toff_t off)
//	{
//	}
//
//		//Deccoding ----------------------------------
//
//	//IStream* g_pTemp = NULL;
//	/*static*/
//	tsize_t	CCITT_ReadProc2 (thandle_t fd, tdata_t buf, tsize_t size)
//	{
//		ULONG ulRead=0;
//		/*if (m_buffDest.GetLength() > 0 )
//		{
//			g_pTemp->Read( buf, size, &ulRead );
//		}*/
///*
//			memcpy( &(m_buffDest.get())[ m_dwOffset ], buf, size);
//*/
//		return ulRead;
//	}
//
///*static*/
//tsize_t	CCITT_WriteProc2 (thandle_t fd, tdata_t buf, tsize_t size)
//{
//	// Allocate some memory if we haven't already 
//	if (m_buffDest.GetLength() == 0)
//		::CreateStreamOnHGlobal(NULL,true,&g_pTemp);
//
//	ULONG ulWritten=0;
//	if (g_pTemp != NULL)
//		g_pTemp->Write( buf, size, &ulWritten );
//
//	return ulWritten;
//}
//// these funcs do nothing because we don't need to do anything for the memory block but the library still insists that they exist.
///*static*/
//toff_t	CCITT_SeekProc2 (thandle_t fd, toff_t off, int i)
//{
//	if( !g_pTemp )
//		return 0;
// //   {	STREAM_SEEK_SET	= 0,
//	//STREAM_SEEK_CUR	= 1,
//	//STREAM_SEEK_END	= 2
//
//	LARGE_INTEGER li = {NULL};
//	ULARGE_INTEGER uliOff = {NULL};
//	li.LowPart = off;
//	g_pTemp->Seek( li, i, &uliOff ); 
//	//switch( i )
//	//{
//	//	case SEEK_CUR://    1
//	//		g_pTemp->Seek( li, i, &uliOff ); 
//	//		break;
//	//	case SEEK_END://    2
//	//		g_pTemp->Seek( li, i, &uliOff ); 
//	//		break;
//	//	case SEEK_SET://    0
//	//		g_pTemp->Seek( li, i, &uliOff ); 
//	//		break;
//	//}
//	return uliOff.LowPart;
//}
///*static*/
//toff_t	CCITT_SeekProc3 (thandle_t fd, toff_t off, int i)
//{
//	if( !fd )
//		return 0;
//	IStream* pStm = *((IStream**)fd);
//	if( !pStm )
//		return 0;
// //   {	STREAM_SEEK_SET	= 0,
//	//STREAM_SEEK_CUR	= 1,
//	//STREAM_SEEK_END	= 2
//
//	LARGE_INTEGER li = {NULL};
//	ULARGE_INTEGER uliOff = {NULL};
//	li.LowPart = off;
//	pStm->Seek( li, i, &uliOff ); 
//	return uliOff.LowPart;
//}
//
///*static*/
//int		CCITT_CloseProc2 (thandle_t fd)
//{
//	return 0;
//}
///*static*/
//toff_t	CCITT_SizeProc2 (thandle_t fd)
//{
//	if( g_pTemp)
//	{
//		HGLOBAL h= NULL;
//		::GetHGlobalFromStream( g_pTemp, &h );
//		if( h )
//			return ::GlobalSize(h);
//
//	}
//	return 0;
//}
///*static*/
///*toff_t	CCITT_SizeProc3 (thandle_t fd)
//{
//	if( !fd )
//		return 0;
//	IStream* pStm = *((IStream**)fd);
//	if( pStm )
//	{
//		HGLOBAL h= NULL;
//		::GetHGlobalFromStream( pStm, &h );
//		if( h )
//			return ::GlobalSize(h);
//
//	}
//	return 0;
//}*/
//};

CTiffMgr::CTiffMgr(void)
{
}
//
//CTiffMgr::~CTiffMgr(void)
//{
//}

// TODO Test this all out!! Either change to use CSharedBuff as streams or just have stream* like ZLib

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EncodeG4Block
////
void CTiffMgr::EncodeG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iBPP, int iWidth, int iHeight, int iComp  )
{
	buffDest.Clear();

	if( buffSrc.GetLength()==0 )
		throw Pdf::Exception::pdfException( "EncodeG4Block: No source data provided." );

	// tell the tiff lib that we are going to use some callbacks because we're gonna be all in-memory
	// by using this func that is equivalent to opening a file.
	try
	{
		//CEnDecodeProcs procs(buffDest);

		byte* pInBuf = &buffSrc.get()[0];   // pointer to the first element in arr
		size_t dwInLen = buffSrc.GetLength();

		// this will "open" a new TIFF.  The procs will write to the dest buffer.  Only some of the procs really do anything since we are just writing one thing after another
		std::ostringstream o( std::ios_base::out );
		TIFF* fpTiff = TIFFStreamOpen("dummy", &o);
					/*TIFFClientOpen ("dummy", "w", (thandle_t)0,
					&procs.CCITT_ReadProc, &procs.CCITT_WriteProc, &procs.CCITT_SeekProc,
					&procs.CCITT_CloseProc, &procs.CCITT_SizeProc, &procs.CCITT_MapFileProc, &procs.CCITT_UnmapFileProc);*/

		// Write some tiff tags to the "file"
		if( fpTiff )
		{
			TIFFSetField( fpTiff, TIFFTAG_IMAGEWIDTH, iWidth );
			TIFFSetField( fpTiff, TIFFTAG_IMAGELENGTH, iHeight );
			TIFFSetField( fpTiff, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);
			TIFFSetField( fpTiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField( fpTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			// Mono
			TIFFSetField( fpTiff, TIFFTAG_BITSPERSAMPLE, 1);
			TIFFSetField( fpTiff, TIFFTAG_SAMPLESPERPIXEL, 1);
			// Grayscale - cannot be done for G4/G3 - just here as an example.
	//		TIFFSetField( fpTiff, TIFFTAG_BITSPERSAMPLE, 8);
	//		TIFFSetField( fpTiff, TIFFTAG_SAMPLESPERPIXEL, 1);

			// The final chunk of image data.
			size_t dw = TIFFWriteEncodedStrip( fpTiff, 0, (tdata_t)pInBuf, dwInLen);

			// all done! Now we can close the file 
			TIFFClose( fpTiff );

			// ToArray...
			std::streamsize siz = o.tellp();

			if( siz > 0 )
			{
				buffDest.Resize( siz, false );
				memcpy( buffDest.get(), o.rdbuf(), siz );
			}
	/*#ifdef _DEBUG
			size_t dwCompressedSize = siz;//buffDest.GetLength();//(g_hMem != 0 ? ::GlobalSize( g_hMem ) : 0);
			wchar_t szBuff[1024]=L"";
			_swprintf( szBuff, L"\t\tOriginal image size %d - compressed size %d\n", dwInLen, dwCompressedSize );
			OutputDebugStringW( szBuff );
			if( dwCompressedSize > dwInLen )
				OutputDebugString( L"\t\t\tSHIT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
	#endif*/
		}
	}
	catch(...)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DecodeG4Block
////
void CTiffMgr::DecodeG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iWidth, int iHeight, int BPP, int K )
{
	buffDest.Clear();

	if( buffSrc.GetLength()==0 )
		throw Pdf::Exception::pdfException( "DecodeG4Block: No source data provided." );

	// tell the tiff lib that we are going to use some callbacks because we're gonna be all in-memory
	// by using this func that is equivalent to opening a file.
	try
	{
		//CEnDecodeProcs procs(buffDest);

		byte* pInBuf = &buffSrc.get()[0];   // pointer to the first element in arr
		size_t dwInLen = buffSrc.GetLength();

		std::ostringstream o( std::ios_base::out );
		TIFF* fpTiff = TIFFStreamOpen("dummy", &o);
					/*TIFFClientOpen ("dummy", "w", (thandle_t)&buffDest,
					&(procs.CCITT_ReadProc), &procs.CCITT_WriteProc2, &procs.CCITT_SeekProc2,
					&procs.CCITT_CloseProc, &procs.CCITT_SizeProc2, &procs.CCITT_MapFileProc, &procs.CCITT_UnmapFileProc);
					*/
		if( fpTiff )
		{
			int iComp = COMPRESSION_CCITTFAX4, iBPS = 1, iSPP = 1;
			TIFFSetField( fpTiff, TIFFTAG_IMAGEWIDTH, iWidth );
			TIFFSetField( fpTiff, TIFFTAG_IMAGELENGTH, iHeight );
			TIFFSetField( fpTiff, TIFFTAG_COMPRESSION, K < 0 ? COMPRESSION_CCITTFAX4 : COMPRESSION_CCITTFAX3 );
			TIFFSetField( fpTiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField( fpTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			// Mono
			TIFFSetField( fpTiff, TIFFTAG_BITSPERSAMPLE, 1);
			TIFFSetField( fpTiff, TIFFTAG_SAMPLESPERPIXEL, 1);

			size_t dw = TIFFWriteRawStrip( fpTiff, 0, (tdata_t)pInBuf, dwInLen);

			//
			::TIFFWriteDirectory(fpTiff);

			//
			TIFFClose( fpTiff );

			// ToArray...
			std::streamsize siz = o.tellp();

			if( siz > 0 )
			{
				buffDest.Resize( siz, false );
				memcpy( buffDest.get(), o.rdbuf(), siz );
			}
		}

	}
	catch(...)
	{
		cout << L"TiffDecompress ERROR!!!!!!!!!!!!\n";
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TiffFromG4Block
////
void CTiffMgr::TiffFromG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iWidth, int iHeight, int BPP, int K )
{
	buffDest.Clear();

	if( buffSrc.GetLength()==0 )
		throw Pdf::Exception::pdfException( "TiffFromG4Block: No source data provided." );

	// tell the tiff lib that we are going to use some callbacks because we're gonna be all in-memory
	// by using this func that is equivalent to opening a file.
	try
	{
		//CEnDecodeProcs procs(buffDest);

		byte* pInBuf = &buffSrc.get()[0];   // pointer to the first element in arr
		size_t dwInLen = buffSrc.GetLength();

		std::ostringstream o( std::ios_base::out );
		TIFF* fpTiff = TIFFStreamOpen("dummy", &o);
					/*TIFFClientOpen ("dummy", "w", (thandle_t)0,
					&procs.CCITT_ReadProc, &procs.CCITT_WriteProc2, &procs.CCITT_SeekProc2,
					&procs.CCITT_CloseProc, &procs.CCITT_SizeProc2, &procs.CCITT_MapFileProc, &procs.CCITT_UnmapFileProc);*/
		if( fpTiff )
		{
			int iComp = COMPRESSION_CCITTFAX4, iBPS = 1, iSPP = 1;
			TIFFSetField( fpTiff, TIFFTAG_IMAGEWIDTH, iWidth );
			TIFFSetField( fpTiff, TIFFTAG_IMAGELENGTH, iHeight );
			TIFFSetField( fpTiff, TIFFTAG_COMPRESSION, K < 0 ? COMPRESSION_CCITTFAX4 : COMPRESSION_CCITTFAX3 );
			TIFFSetField( fpTiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField( fpTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			// Mono
			TIFFSetField( fpTiff, TIFFTAG_BITSPERSAMPLE, 1);
			TIFFSetField( fpTiff, TIFFTAG_SAMPLESPERPIXEL, 1);

			size_t dw = TIFFWriteRawStrip( fpTiff, 0, (tdata_t)pInBuf, dwInLen);
			::TIFFWriteDirectory(fpTiff);

			//
			TIFFClose( fpTiff );


			// ToArray...
			std::streamsize siz = o.tellp();

			if( siz > 0 )
			{
				buffDest.Resize( siz, false );
				memcpy( buffDest.get(), o.rdbuf(), siz );
			}
		}
	}
	catch(...)
	{
		//OutputDebugStringW( L"TiffFromG4Block ERROR!!!!!!!!!!!!\n" );
	}
}

};
};
