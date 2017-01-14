#include "stdafx.h"
#include "NameTree.h"
#include "Enum.h"
#include "Document.h"


using namespace Pdf::Geometry;

namespace Pdf {

pdfNameTree::pdfNameTree(void)// : pdfObject()
{
}

pdfNameTree::pdfNameTree(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsNameTree = Get("Type").Str() == "NameTree";
		if( !bIsNameTree )
			throw pdfObjectInvalidEx();
	}
}

pdfNameTree::pdfNameTree(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfNameTree::~pdfNameTree(void)
{
}

pdfNameTree& pdfNameTree::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsNameTree = Get("Type").Str() == "NameTree";
		if( !bIsNameTree )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

};// namespace Pdf
