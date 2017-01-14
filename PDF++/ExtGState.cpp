#include "stdafx.h"
#include "ExtGState.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfExtGState::pdfExtGState(void)// : pdfObject()
{
}

pdfExtGState::pdfExtGState(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsExtGState = Get("Type").Str() == "ExtGState";
		if( !bIsExtGState )
			throw pdfObjectInvalidEx();
	}
}

pdfExtGState::pdfExtGState(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfExtGState::~pdfExtGState(void)
{
}

pdfExtGState& pdfExtGState::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsExtGState = Get("Type").Str() == "ExtGState";
		if( !bIsExtGState )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
