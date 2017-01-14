#include "stdafx.h"
#include "FlateFilter.h"
#include "ZLibMgr.h"
#include "streams.h"

///////////////////////
namespace Pdf {
namespace Filter {

pdfFlate::pdfFlate(void)
{
}

pdfFlate::~pdfFlate(void)
{
}

// pdfFilter
void pdfFlate::Transform( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bDecode )
{
	buffDest.Clear();
	bytestream stmBuffDest;

	Transform( buffSrc, *((std::ostream*)&stmBuffDest), bDecode );

	if( stmBuffDest.GetLength() > 0 )
	{
		buffDest.Resize( stmBuffDest.GetLength() );
		//stmBuffDest.seekp(0, std::ios::beg);
		stmBuffDest.seekg(0, std::ios::beg);
		stmBuffDest.read( buffDest.get(), buffDest.GetLength() );
	}
}

void pdfFlate::Transform( const CSharedBuff& buffSrc, std::ostream& stmDest, bool bDecode )
{
	ZLib zlib;

	unsigned int nBestSize = bDecode ?  min( (size_t)(1<<12), buffSrc.GetLength()*2) :
										buffSrc.GetLength() + 12 + (unsigned int)((double)buffSrc.GetLength() *.01);

	std::unique_ptr<byte[]> tmpOut( new byte[nBestSize] );

	byte* pIn = &buffSrc.get()[0]; 
	byte* pOut = tmpOut.get();
	size_t ulDest = nBestSize;

	int iErr=0;//, iPasses=0;
	if( !bDecode )
	{
		if( (iErr=zlib.Compress(  pOut, &ulDest, pIn, buffSrc.GetLength() ) ) !=0 )
		{
			// TODO: add some exceptions here or something.
			/*switch( iErr )
			{
			case X:
				throw
			}*/

		}
		else
			stmDest.write( (const char*)pOut, ulDest );
	}
	else
	{
		unsigned char* pStoppedAt = pIn;

		while( (int)Z_BUF_ERROR == (iErr=zlib.Decompress( pOut, &ulDest, pIn, buffSrc.GetLength() - (pStoppedAt-pIn), &pStoppedAt )) || iErr == (int)Z_OK )
		{
			//iPasses++;
			unsigned int iLen = ulDest > nBestSize?nBestSize: ulDest;//- memOut->Length;
			stmDest.write( (const char*)pOut, iLen );

			if(iErr == Z_OK)
				break;

			ulDest = nBestSize;
		}
	}
}

FilterType pdfFlate::GetType() const
{
	return FilterType::FlateDecode;
};

const char* pdfFlate::ToString()
{
	return "FlateDecode";
}

}; //namespace Filter 
}; //namespace Pdf 
