#pragma once
#include <memory>
#include "Atom.h"
#include "Int.h"
#include "Int64.h"
#include "Real.h"
#include "Bool.h"
#include "String.h"
#include "Name.h"
//#include "Dictionary.h"
//#include "Blob.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfArray : public pdfAtom
{
public:
	pdfArray(void);
	pdfArray(const pdfAtom& src);
	pdfArray(pdfDocument& doc, const size_t iStartingCount=0);
	virtual ~pdfArray(void);

	pdfArray& operator=(const pdfAtom& src);
	inline pdfArray& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}


	pdfAtom operator[]( const size_t i );

	pdfAtom Add( const pdfAtom& atm ); 
	//void Insert(int Index, const pdfAtom& atm);
	pdfInt Add( int Val );
	pdfInt64 Add( int64 Val );
	pdfReal Add( float Val, int iPrecision=8 );
	pdfReal Add( double Val, int iPrecision = 8 );
	pdfBool Add( bool Val );
	pdfAtom Add( const char* Val, bool bIsName = true );

	pdfAtom Get( const size_t i );
	pdfAtom Set( const size_t idx, const pdfAtom& atm );
	size_t GetCount() const;
	void Remove( const size_t idx );
	//void Clear();

	template< class _Fn1> 
	inline _Fn1 ForEach(_Fn1 _Func)
	{
		if( m_data != nullptr )
		{
			// perform function for each element
			for( auto it = m_data->m_vecArray.begin(); it!=m_data->m_vecArray.end(); ++it )
			{
				pdfAtom atm;
				atm.m_data = *(it);
				atm = atm.Dereference();
				_Func( atm );
			}
			//std::for_each( m_data->m_vecArray.begin(), m_data->m_vecArray.end(), _Func );
		}
		return (_Func);
	};
protected:
	void TakeOwnership(const pdfAtom& atm);
	// Used for references when we have to affect its reference count (like when adding to a collection)
	/*size_t AddRef(pdfAtom atmNew)
	{
		if ( atmNew.IsIndirect() )
			atmNew = atmNew.GetRef();
		if ( atmNew.GetType() == Reference )
		{
			pdfReference refer(atmNew);
			refer.IncrementCount();
			return refer.GetReferenceCount();
		}
		return 1;
	}
	size_t Release(pdfAtom atmNew)
	{
		if ( atmNew.IsIndirect() )
			atmNew = atmNew.GetRef();
		if ( atmNew.GetType() == Reference )
		{
			pdfReference refer(atmNew);
			refer.DecrementCount();
			return refer.GetReferenceCount();
		}
		return 1;
	}*/
};

};// namespace Exception
