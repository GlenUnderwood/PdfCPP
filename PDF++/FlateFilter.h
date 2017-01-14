#pragma once
#include "AtomData.h"
#include "Filter.h"

#include "ExportDef.h"

namespace Pdf {
namespace Filter {

class PDF_API pdfFlate : public pdfFilter
{
public:
	pdfFlate(void);
	virtual ~pdfFlate(void);

	// pdfFilter
	void Transform( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bDecode );
	void Transform( const CSharedBuff& buffSrc, std::ostream& buffDest, bool bDecode );
/*	void Encode( const CSharedBuff& buffSrc, CSharedBuff& buffDest );
	void Encode( const CSharedBuff& buffSrc, std::ostream& stmDest );
	void Decode( const CSharedBuff& buffSrc, CSharedBuff& buffDest );
	void Decode( const CSharedBuff& buffSrc, std::ostream& stmDest );*/
	FilterType GetType() const;
	const char* ToString();
};

}; //namespace Filter 
}; //namespace Pdf 
