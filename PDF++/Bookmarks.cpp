#include "stdafx.h"
#include "Bookmarks.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfBookmarks::pdfBookmarks(void)// : pdfObject()
{
}

pdfBookmarks::pdfBookmarks(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsBookmarks = Get("Type").Str() == "Bookmarks";
		if( !bIsBookmarks )
			throw pdfObjectInvalidEx();
	}
}

pdfBookmarks::pdfBookmarks(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfBookmarks::~pdfBookmarks(void)
{
}

pdfBookmarks& pdfBookmarks::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsBookmarks = Get("Type").Str() == "Bookmarks";
		if( !bIsBookmarks )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
