#include "stdafx.h"
#include "Int.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfInt::pdfInt(void) : pdfAtom()
{
	//m_data->m_type = AtomType::Intean;
}

pdfInt::pdfInt(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::Integer )
		throw pdfTypeMismatchException();
}

pdfInt::pdfInt(pdfDocument& doc, int i) : pdfAtom(doc)
{
	//pdfAtom::operator=( i );
	m_data->m_type = Integer;
	m_data->m_Val.iVal = i;
}

pdfInt::~pdfInt(void)
{
}

pdfInt& pdfInt::operator=(const pdfAtom& src)
{
	if( this != &src )
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Integer )
			throw pdfTypeMismatchException();
		pdfAtom::operator=(src);
	}
	return *this;
}

pdfInt pdfInt::operator=(int src){
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 
	m_data->m_Val.iVal = src;
	return *this;
}
//pdfInt::operator int(){
//	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
//		throw pdfDocUndefinedException(); 
//	return m_data->m_Val.iVal;
//}

};// namespace Exception
