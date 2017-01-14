#include "stdafx.h"
#include "Object.h"
#include "Enum.h"
#include "Model.h"
#include "Document.h"

namespace Pdf {

pdfObject::pdfObject(void)
{
	//m_doc = nullptr;
}

pdfObject::pdfObject(const pdfAtom& src)
{
	//m_doc = nullptr;
	operator=(src);
}


pdfObject::~pdfObject(void)
{
	Reset();
}

pdfObject& pdfObject::operator=(const pdfAtom& src)
{
	m_atm = pdfAtom();
	//m_doc = nullptr;

	pdfAtom deref( src.IsNull() ? src : src.Dereference() );
	if (!deref.GetType() != AtomType::Null // setting to 'nothing' is ok
			&& deref.GetType() != Dictionary && deref.GetType() != AtomType::Stream && deref.GetType() != AtomType::Array )
		throw pdfTypeMismatchException();//pdfMgrException( o.GetType() + " is not a dictionary." );

	if (!src.IsNull())
	{
		m_atm = deref;
		//m_doc = src.GetDocument();
	}

	return *this;
}
/*
pdfObject& pdfObject::operator=(const pdfDictionary& src)
{
	operator=( (pdfAtom)src );
	return *this;
}
*/

//operator pdfDictionary();
//operator pdfStream();
//operator pdfArray();

pdfAtom pdfObject::operator[]( const size_t i )
{
	if (!m_atm.IsNull() )
	{
		AtomType type = m_atm.GetType();

		switch( type )
		{
		case AtomType::Dictionary:
			return pdfDictionary(m_atm)[ i ];
			break;
		case AtomType::Stream:
			return pdfBlob(m_atm)[ i ];
			break;
		case AtomType::Array:
			return pdfArray(m_atm)[ i ];
			break;
		default:
			throw pdfTypeMismatchException();
		}
	}
	return pdfAtom();
}

pdfAtom pdfObject::operator[]( const char* i )
{
	if (!m_atm.IsNull() )
	{
		AtomType type = m_atm.GetType();

		switch( type )
		{
		case AtomType::Dictionary:
			return pdfDictionary(m_atm)[ i ];
			break;
		case AtomType::Stream:
			return pdfBlob(m_atm)[ i ];
			break;
		default:
			throw pdfTypeMismatchException();
		}
	}
	return pdfAtom();
}

pdfAtom pdfObject::operator[]( const std::string& i )
{
	if (!m_atm.IsNull() )
	{
		AtomType type = m_atm.GetType();

		switch( type )
		{
		case AtomType::Dictionary:
			return pdfDictionary(m_atm)[ i.c_str() ];
			break;
		case AtomType::Stream:
			return pdfBlob(m_atm)[ i.c_str() ];
			break;
		default:
			throw pdfTypeMismatchException();
		}
	}
	return pdfAtom();
}

pdfDocument pdfObject::GetDocument() const { return m_atm.GetDocument(); };

pdfDictionary pdfObject::GetDict() const
{
	if (m_atm.IsNull())
		throw pdfDocUndefinedException();
	if (m_atm.GetType() != Dictionary && m_atm.GetType() != AtomType::Stream )
		throw pdfTypeMismatchException();//pdfMgrException( o.GetType() + " is not a dictionary." );
	return m_atm;
}

pdfBlob pdfObject::GetStream() const
{
	if (m_atm.IsNull())
		throw pdfDocUndefinedException();
	if (  m_atm.GetType() != AtomType::Stream )
		throw pdfTypeMismatchException();//pdfMgrException( o.GetType() + " is not a dictionary." );
	return m_atm;
}

pdfArray pdfObject::GetArray() const
{
	if (m_atm.IsNull())
		throw pdfDocUndefinedException();
	if (m_atm.GetType() != AtomType::Array )
		throw pdfTypeMismatchException();//pdfMgrException( o.GetType() + " is not a dictionary." );
	return m_atm;
}

pdfAtom pdfObject::GetAtom() const throw()
{
	return m_atm;
}

void pdfObject::Reset()
{
	//m_doc = nullptr;
	m_atm = nullptr;//pdfAtom();
}

};// namespace Exception
