#pragma once
#include "AtomData.h"
#include "Filter.h"

#include "ExportDef.h"

namespace Pdf {
namespace Filter {

class PDF_API pdfCCITT : public pdfFilter
{
public:
	pdfCCITT(void);
	virtual ~pdfCCITT(void);

	// pdfFilter
	void Transform( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bDecode );
	void Transform( const CSharedBuff& buffSrc, std::ostream& buffDest, bool bDecode );
/*	void Encode( const CSharedBuff& buffSrc, CSharedBuff& buffDest );
	void Encode( const CSharedBuff& buffSrc, std::ostream& stmDest );
	void Decode( const CSharedBuff& buffSrc, CSharedBuff& buffDest );
	void Decode( const CSharedBuff& buffSrc, std::ostream& stmDest );*/
	FilterType GetType() const { return FilterType::CCITTFaxDecode; };
	const char* ToString();

	inline int GetK() const { return _K; } ;
	inline void SetK( int i ) { _K = i; } ;

	inline int GetBPP() const { return _BPP; } ;
	inline void SetBPP( int i ) { _BPP = i; } ;

	inline int GetHeight() const { return _Height; } ;
	inline void SetHeight( int i ) { _Height = i; } ;

	inline int GetWidth() const { return _Width; } ;
	inline void SetWidth( int i ) { _Width = i; } ;

	/// <summary>
	/// if true then the decoding process will just return the G4 block wrapper by a TIFF header, effectively a tif file.
	/// </summary>
	inline bool GetDecodeToTIFF() const { return _DecodeToTIFF; } ;
	inline void SetDecodeToTIFF( bool i ) { _DecodeToTIFF = i; } ;

protected:
	int _K, _BPP, _Height, _Width ;
	bool _DecodeToTIFF;

	void EncodeG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iComp );

	void DecodeG4Block(	const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bTiffHeader );
	//void TiffFromG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest );
};

}; //namespace Filter 
}; //namespace Pdf 
