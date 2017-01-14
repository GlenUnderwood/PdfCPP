#include "stdafx.h"
#include "Int64.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfInt64::pdfInt64(void) : pdfAtom()
{
	//m_data->m_type = AtomType::Int64ean;
}

pdfInt64::pdfInt64(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::Integer64 )
		throw pdfTypeMismatchException();
}

pdfInt64::pdfInt64(pdfDocument& doc, int64 i) : pdfAtom(doc)
{
	//pdfAtom::operator=( i );
	m_data->m_type = Integer64;
	m_data->m_Val.i64Val = i;
}

pdfInt64::~pdfInt64(void)
{
}

pdfInt64& pdfInt64::operator=(const pdfAtom& src)
{
	if( this != &src )
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Integer64 )
			throw pdfTypeMismatchException();
		pdfAtom::operator=(src);
	}
	return *this;
}

pdfInt64 pdfInt64::operator=(int64 src){
	CheckIsValid(); 

	m_data->m_Val.i64Val = src;
	return *this;
}
//pdfInt64::operator int64(){
//	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
//		throw pdfDocUndefinedException(); 
//	return m_data->m_Val.i64Val;
//};

};// namespace Exception
