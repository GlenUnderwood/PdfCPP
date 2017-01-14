#include "stdafx.h"
#include "FormXObject.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfFormXObject::pdfFormXObject(void)// : pdfObject()
{
}

pdfFormXObject::pdfFormXObject(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsFormXObject = Get("Type").Str() == "FormXObject";
		if( !bIsFormXObject )
			throw pdfObjectInvalidEx();
	}
}

pdfFormXObject::pdfFormXObject(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfFormXObject::~pdfFormXObject(void)
{
}

pdfFormXObject& pdfFormXObject::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsFormXObject = Get("Type").Str() == "FormXObject";
		if( !bIsFormXObject )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
