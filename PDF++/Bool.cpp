#include "stdafx.h"
#include "Bool.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfBool::pdfBool(void) //: pdfAtom()
{
	//m_data->m_type = AtomType::Boolean;
}

pdfBool::pdfBool(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Boolean )
		throw pdfTypeMismatchException();
}

pdfBool::pdfBool(pdfDocument& doc, bool i) : pdfAtom(doc)
{
	//pdfAtom::operator=( i );
	m_data->m_type = Boolean;
	m_data->m_Val.bVal = i;
}

pdfBool::~pdfBool(void)
{
}

pdfBool& pdfBool::operator=(const pdfAtom& src)
{
	if( this != &src )
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Boolean )
			throw pdfTypeMismatchException();
		pdfAtom::operator=(src);
	}
	return *this;
}

pdfBool pdfBool::operator=(bool src){
	CheckIsValid(); 
	m_data->m_Val.bVal = src;
	return *this;
}
//pdfBool::operator bool(){
//	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
//		throw pdfDocUndefinedException(); 
//	return m_data->m_Val.bVal;
//}

};// namespace Exception
