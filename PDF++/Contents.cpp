#include "stdafx.h"
#include "Contents.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

// Content objects can be either an array or a Stream
//#define CHECK_OBJECT \ pdfAtom::CheckIsValid(); \
//	if (GetType() != AtomType::Stream && GetType() != AtomType::Array)\
//		throw pdfTypeMismatchException();

pdfContents::pdfContents(void)// : pdfObject()
{
}

pdfContents::pdfContents(const pdfAtom& src) //: pdfAtom(src)
{
	operator=(src);
}

//pdfContents::pdfContents( pdfDocument& doc ) : pdfAtom( doc )
//{
//}

pdfContents::~pdfContents(void)
{
}


void pdfContents::CheckIsValid() const
{
	// call the base first.
	m_atm.CheckIsValid();

	if (!m_atm.IsStream() && !m_atm.IsArray())
		throw pdfTypeMismatchException();
}

pdfContents& pdfContents::operator=(const pdfAtom& src)
{
	m_atm = src;

	if (!m_atm.IsNull())// being set to null is OK
	{
		// Content objects can be either an array or a Stream
		CheckIsValid();
	}
	return *this;
}


};// namespace Pdf
