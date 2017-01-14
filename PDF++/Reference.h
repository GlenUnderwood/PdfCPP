#pragma once
#include <memory>
#include "Atom.h"
#include "Constants.h"
#include "pdfException.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfReference : public pdfAtom
{
public:
	pdfReference(void);
	pdfReference(const pdfAtom& ref);
	//pdfReference(const pdfReference& ref);
	pdfReference( const pdfDocument& doc, pdfObjId* pID = nullptr, pdfAtom* atm = nullptr, int64 iFilePos = MinValueInt64);
	virtual ~pdfReference(void);

	pdfReference& operator=(const pdfAtom& src);
	inline pdfReference& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}


	void Serialize(  std::ostream& stm )
	{
		stm << " R";
	};

	pdfAtom GetValue() const;

	inline int64 GetFilePos() const
	{
		return m_data->m_FilePos;
	};

	const size_t GetReferenceCount() const
	{
		if( m_data==nullptr || m_data->m_doc.expired() )
			throw Pdf::Exception::pdfDocUndefinedException();
		return m_data->m_iRefs;
	};

	inline void IncrementCount()
	{
		if( m_data==nullptr || m_data->m_doc.expired() )
			throw Pdf::Exception::pdfDocUndefinedException();
		m_data->m_iRefs++;
	};

	inline void DecrementCount()
	{
		if( m_data==nullptr || m_data->m_doc.expired() )
			throw Pdf::Exception::pdfDocUndefinedException();
		if( m_data->m_iRefs > 0 )
			m_data->m_iRefs--;
		//return m_data->m_iRefs;
	};
protected:

};

};// namespace Exception
