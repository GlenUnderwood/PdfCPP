#include "stdafx.h"
#include "ImageObj.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfImageObj::pdfImageObj(void)// : pdfObject()
{
}

pdfImageObj::pdfImageObj(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsImageObj = Get("Type").Str() == "ImageObj";
		if( !bIsImageObj )
			throw pdfObjectInvalidEx();
	}
}

pdfImageObj::pdfImageObj(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfImageObj::~pdfImageObj(void)
{
}

pdfImageObj& pdfImageObj::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsImageObj = Get("Type").Str() == "ImageObj";
		if( !bIsImageObj )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
