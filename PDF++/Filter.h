#pragma once
#include "ExportDef.h"
#include "AtomData.h"


// TODO:  filter objs should hold on to the atom?  Even if there's no extra info. ")

namespace Pdf {
namespace Filter {

class PDF_API pdfFilter
{
public:
	//pdfFilter(void);
	//virtual ~pdfFilter(void);

	virtual void Transform( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bDecode ) = 0;
	virtual void Transform( const CSharedBuff& buffSrc, std::ostream& buffDest, bool bDecode ) = 0;
	//virtual void Encode( const CSharedBuff& buffSrc, std::ostream& stmDest ) = 0;
	//virtual void Decode( const CSharedBuff& buffSrc, CSharedBuff& buffDest ) = 0;
	//virtual void Decode( const CSharedBuff& buffSrc, std::ostream& stmDest ) = 0;
	virtual FilterType GetType() const  = 0;//{ return FilterType::NoFilter; };
	virtual const char* ToString() = 0;
};

}; //namespace Filter 
}; //namespace Pdf 
