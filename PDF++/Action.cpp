#include "stdafx.h"
#include "Action.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfAction::pdfAction(void)// : pdfObject()
{
}

pdfAction::pdfAction(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsAction = Get("Type").Str() == "Action";
		if( !bIsAction )
			throw pdfObjectInvalidEx();
	}
}

pdfAction::pdfAction(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfAction::~pdfAction(void)
{
}

pdfAction& pdfAction::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsAction = Get("Type").Str() == "Action";
		if( !bIsAction )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
