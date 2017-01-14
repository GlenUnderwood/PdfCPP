#include "stdafx.h"
#include "Destination.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfDestination::pdfDestination(void)// : pdfObject()
{
}

pdfDestination::pdfDestination(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsDestination = Get("Type").Str() == "Destination";
		if( !bIsDestination )
			throw pdfObjectInvalidEx();
	}
}

pdfDestination::pdfDestination(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfDestination::~pdfDestination(void)
{
}

pdfDestination& pdfDestination::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsDestination = Get("Type").Str() == "Destination";
		if( !bIsDestination )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
