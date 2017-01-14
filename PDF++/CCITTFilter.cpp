#include "stdafx.h"
#include "CCITTFilter.h"

#include <sstream>

// For Tiff
// TODO what to do when on Windows but using the gnu compiler? (like in Eclipse)
#ifdef _WINDOWS
#define USE_WIN32_FILEIO
#define __WIN32__
#include <tiffio.h>
#include <tiffio.hxx>

// TODO Fix this
#ifdef _DEBUG
	#pragma comment(lib,"libTiffD.lib")
#else
	#pragma comment(lib,"libTiff.lib")
#endif
#else
#include <tiffio.h>
#include <tiffio.hxx>
#endif

///////////////////////
namespace Pdf {
namespace Filter {

pdfCCITT::pdfCCITT(void)
{
	_K = _Height = _Width = 0;
	_BPP = 1;
	_DecodeToTIFF = false;
}

pdfCCITT::~pdfCCITT(void)
{
}


const char* pdfCCITT::ToString()
{
	return "CCITTFaxDecode";
}

// pdfFilter
// TODO either convert this or overload with versions that take standard streams.
void pdfCCITT::Transform( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bDecode )
{
	if( !bDecode )
	{
		EncodeG4Block(  buffSrc, buffDest, 0 );
	}
	else
	{
		if( _DecodeToTIFF )
			// Wrap the G4 block in a TIFF header so it can just be loaded by any graphics package.
			/*TiffFromG4Block*/DecodeG4Block( buffSrc, buffDest, true ); 
		else// Decode the block into a DIB (with no header, obviously, just the bits)
			DecodeG4Block( buffSrc, buffDest, false );
	#ifdef _DEBUG
		/*std::ofstream fout;
		try
		{
			fout.open( "C:\\users\\glen\\desktop\\Contents.tif", std::ios::out | std::ios::trunc);
			fout.write( (const char*)buffDest.get(), buffDest.GetLength() );
			fout.flush();
		}
		catch(...)
		{
		}
		if( fout.is_open() )
			fout.close();*/
	#endif
	}
}

void pdfCCITT::Transform( const CSharedBuff& buffSrc, std::ostream& buffDest, bool bDecode )
{
	throw pdfNotImplException();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EncodeG4Block
////
void pdfCCITT::EncodeG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iComp  )
{
	buffDest.Clear();

	if (_Width == 0 || _Height == 0 || _BPP != 1 || _K>0 )
		throw pdfException("Bad CCITT args");
	if( buffSrc.GetLength()==0 )
		throw pdfException( "EncodeG4Block: No source data provided." );

	// tell the tiff lib that we are going to use some callbacks because we're gonna be all in-memory
	// by using this func that is equivalent to opening a file.
	try
	{
		//CEnDecodeProcs procs(buffDest);

		byte* pInBuf = &buffSrc.get()[0];   // pointer to the first element in arr
		size_t dwInLen = buffSrc.GetLength();

		// this will "open" a new TIFF.  The procs will write to the dest buffer.  Only some of the procs really do anything since we are just writing one thing after another
		obytestream stmBuffDest( std::ios_base::out );
		TIFF* fpTiff = TIFFStreamOpen("dummy", (std::ostream*)&stmBuffDest);
					/*TIFFClientOpen ("dummy", "w", (thandle_t)0,
					&procs.CCITT_ReadProc, &procs.CCITT_WriteProc, &procs.CCITT_SeekProc,
					&procs.CCITT_CloseProc, &procs.CCITT_SizeProc, &procs.CCITT_MapFileProc, &procs.CCITT_UnmapFileProc);*/

		// Write some tiff tags to the "file"
		if( fpTiff )
		{
			TIFFSetField( fpTiff, TIFFTAG_IMAGEWIDTH, _Width );
			TIFFSetField( fpTiff, TIFFTAG_IMAGELENGTH, _Height );
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
			std::streamsize siz = stmBuffDest.tellp();
			siz = stmBuffDest.GetLength();
			if( siz > 0 )
			{
				buffDest.Resize( siz, false );
				memcpy( buffDest.get(), stmBuffDest.GetBuffer(), siz );
			}
	#ifdef _DEBUG
			//size_t dwCompressedSize = siz;//buffDest.GetLength();//(g_hMem != 0 ? ::GlobalSize( g_hMem ) : 0);
			//wchar_t szBuff[1024]=L"";
			//_swprintf( szBuff, L"\t\tOriginal image size %d - compressed size %d\n", dwInLen, dwCompressedSize );
			//OutputDebugStringW( szBuff );
			//if( dwCompressedSize > dwInLen )
			//	OutputDebugString( L"\t\t\tSHIT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
	#endif
		}
	}
	catch(...)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DecodeG4Block
////
void pdfCCITT::DecodeG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bTiffHeader )
{
	buffDest.Clear();

	if (_Width == 0 || _Height == 0 || _BPP != 1 || _K>0 )
		throw pdfException( "Bad CCITT args" );
	if( buffSrc.GetLength()==0 )
		throw pdfException( "DecodeG4Block: No source data provided." );

	// tell the tiff lib that we are going to use some callbacks because we're gonna be all in-memory
	// by using this func that is equivalent to opening a file.
	try
	{
		//CEnDecodeProcs procs(buffDest);

		byte* pInBuf = &buffSrc.get()[0];   // pointer to the first element in arr
		size_t dwInLen = buffSrc.GetLength();

		//std::ostringstream o( std::ios_base::out );
		bytestream stmBuffDest;
		//stringstream stmBuffDest;
		TIFF* fpTiff = TIFFStreamOpen("dummy", (std::ostream*)&stmBuffDest);
					/*TIFFClientOpen ("dummy", "w", (thandle_t)&buffDest,
					&(procs.CCITT_ReadProc), &procs.CCITT_WriteProc2, &procs.CCITT_SeekProc2,
					&procs.CCITT_CloseProc, &procs.CCITT_SizeProc2, &procs.CCITT_MapFileProc, &procs.CCITT_UnmapFileProc);
					*/
		if( fpTiff )
		{
			int iComp = COMPRESSION_CCITTFAX4, iBPS = 1, iSPP = 1;
			TIFFSetField( fpTiff, TIFFTAG_IMAGEWIDTH, _Width );
			TIFFSetField( fpTiff, TIFFTAG_IMAGELENGTH, _Height );
			TIFFSetField( fpTiff, TIFFTAG_COMPRESSION, _K < 0 ? COMPRESSION_CCITTFAX4 : COMPRESSION_CCITTFAX3 );
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
			std::streamsize siz = stmBuffDest.tellp();
			//siz = stmBuffDest.GetLength();
			if( siz > 0 )
			{
				bool bCopied = false;
				if( !bTiffHeader )
				{
					// one more step to get JUST the G4 block

					stmBuffDest.seekp( 0, ios::beg );

					fpTiff = TIFFStreamOpen("dummy", (std::istream*)&stmBuffDest );
					/*fpTiff = TIFFClientOpen ("dummy", "r", (thandle_t)&pStm,
						CCITT_ReadProc2, CCITT_WriteProc, CCITT_SeekProc2,
						CCITT_CloseProc, CCITT_SizeProc2, CCITT_MapFileProc, CCITT_UnmapFileProc);*/

					if( fpTiff!=NULL )
					{
						dw = DWORDAlignedBytes(_Width,_BPP)*_Height;
						if( dw > buffDest.GetLength() ) 
							buffDest.Resize( dw, false ) ;
						dw = TIFFReadEncodedStrip( fpTiff, 0, (tdata_t)buffDest.get(), buffDest.GetLength());

						TIFFClose( fpTiff );

						buffDest.Resize( dw, true );
						bCopied = true;
					}
				}
				if( !bCopied )
				{
					buffDest.Resize( siz, false );
					//stmBuffDest.seekp(0, std::ios::beg);
					stmBuffDest.seekg(0, std::ios::beg);
					stmBuffDest.read( buffDest.get(), buffDest.GetLength() );
				}
			}
		}
	}
	catch(...)
	{
		//OutputDebugStringW( L"TiffDecompress ERROR!!!!!!!!!!!!\n" );
		throw pdfException( "TiffDecompress: unhandled exception." );
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TiffFromG4Block
////
/*void pdfCCITT::TiffFromG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest )
{
	buffDest.Clear();

	if (_Width == 0 || _Height == 0 || _BPP != 1 || _K>0 )
		throw pdfException("Bad CCITT args");
	if( buffSrc.GetLength()==0 )
		throw pdfException( "TiffFromG4Block: No source data provided." );

	// tell the tiff lib that we are going to use some callbacks because we're gonna be all in-memory
	// by using this func that is equivalent to opening a file.
	try
	{
		//CEnDecodeProcs procs(buffDest);

		byte* pInBuf = &buffSrc.get()[0];   // pointer to the first element in arr
		size_t dwInLen = buffSrc.GetLength();

		std::ostringstream o( std::ios_base::out );
		TIFF* fpTiff = TIFFStreamOpen("dummy", &o);
					//TIFFClientOpen ("dummy", "w", (thandle_t)0,
					//&procs.CCITT_ReadProc, &procs.CCITT_WriteProc2, &procs.CCITT_SeekProc2,
					//&procs.CCITT_CloseProc, &procs.CCITT_SizeProc2, &procs.CCITT_MapFileProc, &procs.CCITT_UnmapFileProc);
		if( fpTiff )
		{
			int iComp = COMPRESSION_CCITTFAX4, iBPS = 1, iSPP = 1;
			TIFFSetField( fpTiff, TIFFTAG_IMAGEWIDTH, _Width );
			TIFFSetField( fpTiff, TIFFTAG_IMAGELENGTH, _Height );
			TIFFSetField( fpTiff, TIFFTAG_COMPRESSION, _K < 0 ? COMPRESSION_CCITTFAX4 : COMPRESSION_CCITTFAX3 );
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
		OutputDebugStringW( L"TiffFromG4Block ERROR!!!!!!!!!!!!\n" );
	}
}*/

}; //namespace Filter 
}; //namespace Pdf 
