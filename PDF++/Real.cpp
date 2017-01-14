#include "stdafx.h"
#include "Real.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfReal::pdfReal(void) //: pdfAtom()
{
	//m_data->m_type = AtomType::Realean;
}

pdfReal::pdfReal(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::Real )
		throw pdfTypeMismatchException();
}

pdfReal::pdfReal(const pdfDocument& doc, float i, int iPrecision) : pdfAtom(doc)
{
	//pdfAtom::operator=( i );
	m_data->m_type = AtomType::Real;
	m_data->m_Val.dVal = i;
	m_data->m_iPrecision = iPrecision;
}
pdfReal::pdfReal(const pdfDocument& doc, double i, int iPrecision) : pdfAtom(doc)
{
//	pdfAtom::operator=( i );
	m_data->m_type = AtomType::Real;
	m_data->m_Val.dVal = i;
	m_data->m_iPrecision = iPrecision;
}

pdfReal::~pdfReal(void)
{
}

pdfReal& pdfReal::operator=(const pdfAtom& src)
{
	if( this != &src ) 
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Real )
			throw pdfTypeMismatchException();
		pdfAtom::operator=(src);
	}
	return *this;
}

pdfReal& pdfReal::operator=(float src)
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 
	m_data->m_Val.dVal = src;
	return *this;
}
pdfReal& pdfReal::operator=(double src)
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 
	m_data->m_Val.dVal = src;
	return *this;
}
//pdfReal::operator float()
//{
//	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
//		throw pdfDocUndefinedException(); 
//	return m_data->m_Val.dVal;
//};
//pdfReal::operator double()
//{
//	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
//		throw pdfDocUndefinedException(); 
//	return m_data->m_Val.dVal;
//};

int pdfReal::GetPrecision() const
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 
	return m_data->m_iPrecision;
}

void pdfReal::SetPrecision(const int prec)
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 
	m_data->m_iPrecision = prec;
}


};// namespace Exception
