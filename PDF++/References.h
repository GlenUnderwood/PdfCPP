#pragma once
#include <map>
#include <list>
#include <algorithm>

#include "ExportDef.h"

#include "DocData.h"
#include "Reference.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfReferences
{
	friend class CDocData;
	friend class pdfDocument;

public:
	pdfReferences(void);
protected:
	pdfReferences(const CDocData::Ptr& src);
	pdfReferences& operator=(const CDocData::Ptr& src) ;
public:
	pdfReferences(const pdfReferences& src);

	pdfReferences& operator=(const pdfReferences& src) ;
	virtual ~pdfReferences(void);

	virtual pdfReference operator[]( const size_t i );
	virtual pdfReference operator[]( const pdfObjId& i );

	pdfReference Add(pdfAtom atm, int64 iFilePos);
	pdfReference Add(pdfObjId& id, pdfAtom atm, int64 iFilePos);
	pdfReference Add(int iID, unsigned short genID, int64 iFilePos, pdfAtom atm);

	pdfReference Get( const size_t i );
//	void Set( const size_t idx, const pdfAtom& atm );
	size_t GetCount() const;
	void Remove( const size_t idx );

	pdfObjId& NextFreeID();
	void SetNextFreeID(pdfObjId& src);

	void Clear();

	template< class _Fn1> inline
	_Fn1 ForEach(_Fn1 _Func)
	{
		if( m_doc==nullptr )
			throw Pdf::Exception::pdfDocUndefinedException();

		// perform function for each element
		for( auto it = m_doc->m_xref.begin(); it!=m_doc->m_xref.end(); ++it )
		{
			_Func( it->first, it->second );
		}

		return (_Func);
	};

protected:
	void CheckIsValid() const;

	CDocData::Ptr m_doc;
};

};// namespace Exception
