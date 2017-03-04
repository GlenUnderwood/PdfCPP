#include "stdafx.h"
#include "Array.h"
#include "Enum.h"
#include "Document.h"


namespace Pdf {

pdfArray::pdfArray(void) //: pdfAtom()
{
}

pdfArray::pdfArray(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::Array )
		throw pdfTypeMismatchException();
}

pdfArray::pdfArray(pdfDocument& doc, const size_t iStartingCount) : pdfAtom(doc)
{
	CheckIsValid();

	m_data->m_type = AtomType::Array;
	if( iStartingCount>0 )
	{
		m_data->m_vecArray.resize(iStartingCount);

		// fill with a bunch of atoms that at least have a connection to the document (but will still be considered null/undefined)
		std::for_each( m_data->m_vecArray.begin(), m_data->m_vecArray.end(),
				[&] (CAtomData::Ptr& ptr) {
					ptr.reset( new CAtomData( m_data->m_doc ) );
				});
	}
}

pdfArray::~pdfArray(void)
{
}

pdfArray& pdfArray::operator=(const pdfAtom& src)
{
	if( this != &src )
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Array )
			throw pdfTypeMismatchException();

		pdfAtom::operator=(src);
	}
	return *this;
}

pdfAtom pdfArray::operator[]( const size_t idx )
{
	return Get(idx);
}
/*
void pdfArray::Add( pdfAtom atm )
{
	if( m_data==nullptr )
		throw pdfDocUndefinedException(); 
	m_data->m_vecArray.push_back( atm );
}
*/
pdfAtom pdfArray::Get( const size_t idx )
{
	CheckIsValid();

	if( idx >= m_data->m_vecArray.size() ) 
		throw pdfBadIndexException();

	pdfAtom atm;
	atm.m_data = m_data->m_vecArray[idx];
	return atm.Dereference();
}

pdfAtom pdfArray::Set( const size_t idx, const pdfAtom& atm )
{
	CheckIsValid();

	if (atm.IsNull() )
		throw pdfException("Cannot set NULLs as value items.");
	if( idx >= m_data->m_vecArray.size() ) 
		throw pdfBadIndexException();

	TakeOwnership( atm );

	if ( atm.IsIndirect() )
		m_data->m_vecArray[idx] = atm.GetRef().m_data;
	else
		m_data->m_vecArray[idx] = atm.m_data ;

	return atm;
}

size_t pdfArray::GetCount() const 
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		return 0; 
	return m_data->m_vecArray.size();
}

void pdfArray::Remove( const size_t idx )
{
	CheckIsValid();

	if( idx >= m_data->m_vecArray.size() ) 
		throw pdfBadIndexException();

	std::vector<pdfAtom>::iterator iter;
	size_t i= 0;
	for( auto it = m_data->m_vecArray.begin(); it!=m_data->m_vecArray.end(); ++it,i++ )
		if( i==idx )
		{
			pdfAtom atm;
			atm.m_data = (*it);
			if ( !atm.IsNull() )
			{
				atm.Release();

				if ( !atm.IsIndirect()/* && !( atm is pdfReference )*/ )
				{
					atm.SetParent( pdfAtom() );
					atm.SetKey( nullptr );
				}
			}

			m_data->m_vecArray.erase( it );
			break;
		}
}
/*
void pdfArray::Clear()
{
	if( m_data!=nullptr )
		m_data->m_vecArray.clear();
}*/

pdfAtom pdfArray::Add(const pdfAtom& atm)
{
	//_ASSERT_EXPR(!atm.IsNull(),L"NULL!");

	TakeOwnership( atm );

	if ( atm.IsIndirect() )
		m_data->m_vecArray.push_back( atm.GetRef().m_data );
	else
		m_data->m_vecArray.push_back( atm.m_data  );

	return atm;
}
/*
void pdfArray::Insert(int Index, const pdfAtom& atm)
{
	if ( Index >= 0 && Index <= m_data->m_vecArray.size() )
	{
		TakeOwnership( atm );

		if (atm.IsIndirect())
			m_lst.Insert( Index, atm.GetRef() );
		else
			m_lst.Insert( Index, atm );
	}
	else
		throw pdfBadIndexException();
}*/
pdfInt pdfArray::Add( int Val )
{
	CheckIsValid();
	pdfDocument doc( m_data->m_doc.lock() );
	return Add(  doc.CreateInteger(Val,false) );
}
pdfInt64 pdfArray::Add( int64 Val )
{
	CheckIsValid();
	pdfDocument doc( m_data->m_doc.lock() );
	return Add(  doc.CreateInteger64(Val,false) );
}
pdfReal pdfArray::Add( float Val, int iPrecision )
{
	CheckIsValid();
	pdfDocument doc( m_data->m_doc.lock() );
	return Add(  doc.CreateReal(Val, iPrecision, false) );
}
pdfReal pdfArray::Add( double Val, int iPrecision )
{
	CheckIsValid();
	pdfDocument doc( m_data->m_doc.lock() );
	return Add(  doc.CreateReal(Val, iPrecision, false) );
}
pdfBool pdfArray::Add( bool Val )
{
	CheckIsValid();
	pdfDocument doc( m_data->m_doc.lock() );
	return Add(  doc.CreateBoolean(Val, false) );
}
pdfAtom pdfArray::Add( const char* Val, bool bIsName  )
{
	CheckIsValid();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfAtom atmNew = bIsName ? pdfAtom( doc.CreateName(Val, false)) :
							   pdfAtom( doc.CreateString(Val, StringEncoding::WinAnsiEncoding, false));
	return Add( atmNew );
}

void pdfArray::TakeOwnership(const pdfAtom& atm)
{
	// when storing make sure we use the reference object if it's an indirect object
	pdfAtom atmNew = atm;
	if ( atm.IsIndirect() )
		atmNew = atm.GetRef();

	if ( !atmNew.IsIndirect() && atmNew.GetType() != Reference )//&& typeid(atmNew) != typeid(pdfReference) ) )
//	if ( !atmNew.IsIndirect() /*&& !( atmNew is pdfReference )*/ )
	{
		if ( !atmNew.GetParent().IsNull() )
			throw pdfAlreadyChildException();
		/// Set the "parent", i.e. Dictionary/Stream or Array, that contains this val.  Only for direct objects and 
		/// only when trying to determine the ID/Gen number for purposes of encryption.  Set when object is added to a Dictionary/Stream or Array.
		atmNew.SetParent( *this );
	}
	//
	// remove old
	atmNew.AddRef();//					( atmNew as pdfReference ).RefCount = ( atmNew as pdfReference ).RefCount + 1; 
}

//template< class _Fn1> 
//_Fn1 pdfArray::ForEach(_Fn1 _Func)
//{
//	if( m_data != nullptr )
//	{
//		// perform function for each element
//		for( auto it = m_data->m_vecArray.begin(); it!=m_data->m_vecArray.end(); ++it )
//		{
//			pdfAtom atm;
//			atm.m_data = *(it->get());
//			_Func( atm );
//		}
//		//std::for_each( m_data->m_vecArray.begin(), m_data->m_vecArray.end(), _Func );
//	}
//	return (_Func);
//}

size_t pdfArray::ForEach( EnumArrayAtom _Func )
{
	size_t i = 0;
	if (m_data != nullptr)
	{
		// perform function for each element
		for (auto it = m_data->m_vecArray.begin(); it != m_data->m_vecArray.end(); ++it)
		{
			pdfAtom atm;
			atm.m_data = *(it);
			atm = atm.Dereference();
			if (!_Func( atm ))
				break;
			i++;
		}
	}
	return i;
}; 

size_t pdfArray::ForEach( EnumArrayAtom2 _Func )
{
	size_t i = 0;
	if (m_data != nullptr)
	{
		// perform function for each element
		for (auto it = m_data->m_vecArray.begin(); it != m_data->m_vecArray.end(); ++it)
		{
			pdfAtom atm;
			atm.m_data = *(it);
			atm = atm.Dereference();
			_Func( atm );
			i++;
		}
	}
	return i;
}; 

};// namespace Exception
