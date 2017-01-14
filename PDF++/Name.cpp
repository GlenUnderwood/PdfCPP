#include "stdafx.h"
#include "Name.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfName::pdfName(void) //: pdfAtom()
{
	//m_data->m_type = AtomType::Name;
}
pdfName::pdfName(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::Name )
		throw pdfTypeMismatchException();
}

pdfName::pdfName(const pdfDocument& doc) : pdfAtom(doc)
{
	m_data->m_type = AtomType::Name;
}

pdfName::pdfName(const pdfDocument& doc, const char* i ) : pdfAtom(doc)
{
	m_data->m_type = AtomType::Name;
	m_data->m_strName = i;
}

pdfName::~pdfName(void)
{
}

pdfName& pdfName::operator=(const pdfAtom& src)
{
	if( this != &src )
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Name )
			throw pdfTypeMismatchException();
		pdfAtom::operator=(src);
	}
	return *this;
}

pdfName pdfName::operator=(const char* src)
{
	m_data->m_strName = src;
	return *this;
}
};// namespace Exception
