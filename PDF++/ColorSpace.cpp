#include "stdafx.h"
#include "ColorSpace.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfColorSpace::pdfColorSpace(void)// : pdfObject()
{
}

pdfColorSpace::pdfColorSpace(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsColorSpace = Get("Type").Str() == "ColorSpace";
		if( !bIsColorSpace )
			throw pdfObjectInvalidEx();
	}
}

pdfColorSpace::pdfColorSpace(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfColorSpace::~pdfColorSpace(void)
{
}

pdfColorSpace& pdfColorSpace::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsColorSpace = Get("Type").Str() == "ColorSpace";
		if( !bIsColorSpace )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
