#include "stdafx.h"
#include "Bookmark.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfBookmark::pdfBookmark(void)// : pdfObject()
{
}

pdfBookmark::pdfBookmark(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsBookmark = Get("Type").Str() == "Bookmark";
		if( !bIsBookmark )
			throw pdfObjectInvalidEx();
	}
}

pdfBookmark::pdfBookmark(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfBookmark::~pdfBookmark(void)
{
}

pdfBookmark& pdfBookmark::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsBookmark = Get("Type").Str() == "Bookmark";
		if( !bIsBookmark )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
