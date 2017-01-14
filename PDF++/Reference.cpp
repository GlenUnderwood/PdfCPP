#include "stdafx.h"
#include "Reference.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfReference::pdfReference(void) : pdfAtom()
{
	//m_data->m_type = AtomType::Reference;

}

pdfReference::pdfReference(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::Reference )
		throw pdfTypeMismatchException();
}

pdfReference::pdfReference(const pdfDocument& doc, pdfObjId* pID, pdfAtom* atm , int64 iFilePos) : pdfAtom(doc)
{
	m_data->m_type = AtomType::Reference;

	if( pID != nullptr )
		m_data->m_id = *pID;

	//m_dataEx.reset( new CDataEx() );
	m_data->m_RefTarget	= atm ? atm->m_data : nullptr;
	m_data->m_FilePos = iFilePos;
	m_data->m_iRefs	= 0;


	if (atm != nullptr && !atm->IsNull() )
	{
		atm->SetId( m_data->m_id );
		atm->SetRef( *this );
	}
}

pdfReference::~pdfReference(void)
{
}

//pdfReference pdfReference::operator=(const pdfReference& ref)
//{
//	pdfAtom::operator=( ref );
//
//	//m_dataEx = ref.m_dataEx;
//
//	return *this;
//}
pdfReference& pdfReference::operator=(const pdfAtom& src)
{
	if( this != &src ) 
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Reference )
			throw pdfTypeMismatchException();
		pdfAtom::operator=(src);
	}
	return *this;
}

pdfAtom pdfReference::GetValue() const
{
	pdfAtom atm;
	if( m_data->m_RefTarget )
		atm.m_data = m_data->m_RefTarget;
	return atm;
};


};// namespace Exception
