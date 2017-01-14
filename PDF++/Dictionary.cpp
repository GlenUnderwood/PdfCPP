#include "stdafx.h"
#include "Dictionary.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfDictionary::pdfDictionary(void) : pdfAtom()
{
	//m_data->m_type = AtomType::Dictionary;
}

pdfDictionary::pdfDictionary(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::Dictionary && t != AtomType::Stream )
		throw pdfTypeMismatchException();
}

pdfDictionary::pdfDictionary(pdfDocument& doc) : pdfAtom(doc)
{
	m_data->m_type = AtomType::Dictionary;
}

pdfDictionary::~pdfDictionary(void)
{
}

pdfDictionary& pdfDictionary::operator=(const pdfAtom& src)
{
	if( this != &src )
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Dictionary && t != AtomType::Stream )
			throw pdfTypeMismatchException();

		pdfAtom::operator=(src);
	}
	return *this;
}

pdfAtom pdfDictionary::operator[]( const size_t idx ) const
{
	return Get(idx);
}

pdfAtom pdfDictionary::operator[]( const char* szKey )// const
{
	//  Add if it doesn't exist.
	//pdfAtom a( Get(szKey) );// Get( )doesn't throw (might later, so if it does then call Has() first 
	//if( a.IsNull() )
	//{
	//	pdfAtom atm( pdfDocument(m_data->m_doc.lock()) );
	//	Set( szKey, atm );
	//	a = atm;
	//}
	return Get(szKey);//a;
}

pdfAtom pdfDictionary::operator[]( const pdfObjId& idx )
{
	return Get(idx);
}

pdfAtom pdfDictionary::operator[]( const std::string& sKey )// const
{
	return Get(sKey.c_str());
}


pdfAtom pdfDictionary::Get( const std::string& sKey ) const
{
	return Get(sKey.c_str());
}

pdfAtom pdfDictionary::Get( const pdfObjId& szKey ) const
{
	CheckIsValid();

	pdfAtom atm;
	int i = 0;
	for (auto it = m_data->m_dictItems.begin(); it != m_data->m_dictItems.end(); ++it)
		if (it->second->m_id == szKey)
			atm.m_data = m_data;
	return atm.Dereference();
}

pdfAtom pdfDictionary::Get( const size_t  idx ) const
{
	CheckIsValid(); 

	if( idx >= m_data->m_dictItems.size() ) 
		throw pdfBadIndexException();

	pdfAtom atm;
	int i = 0;
	for( auto it = m_data->m_dictItems.begin(); it != m_data->m_dictItems.end(); ++it )
		if( i++ == idx )
			atm.m_data = m_data;
	return atm.Dereference();
}
pdfAtom pdfDictionary::Set( const size_t idx, const pdfAtom& atm )
{
	CheckIsValid(); 

	if( idx >= m_data->m_dictItems.size() ) 
		throw pdfBadIndexException();
	int i = 0;
	/*std::for_each( m_data->m_dictItems.begin(), m_data->m_dictItems.end(), 
			[&i, &idx] ( CAtomData* p ) { 
							if( i++ == idx )
							{
								TakeOwnership( atm, it->second != nullptr ? it->second->m_sKey.c_str() : nullptr );
								;
							}
				});*/
	for( auto it = m_data->m_dictItems.begin(); it != m_data->m_dictItems.end(); ++it )
		if( i++ == idx )
		{
			TakeOwnership( atm, it->second != nullptr ? it->second->m_sKey.c_str() : nullptr );
			return atm;
		}
	return pdfAtom();
}

pdfAtom pdfDictionary::Get( const char* szKey ) const
{
	CheckIsValid();

	pdfAtom atm;
	auto it = m_data->m_dictItems.find( szKey );
	if (it == m_data->m_dictItems.end())
		return atm;//throw pdfBadIndexException();// TODO already have too much code where this is used and then IsNull() is used on it

	atm.m_data = it->second;
	atm = atm.Dereference();

	return atm;
}

pdfAtom pdfDictionary::Set( const char* szKey, const pdfAtom& atm )
{
	CheckIsValid();
	TakeOwnership( atm, szKey );
	return atm;
}

bool pdfDictionary::Has( const char* szKey ) const throw()
{
	CheckIsValid(); 
	return ( m_data->m_dictItems.find(szKey) != m_data->m_dictItems.end() );
}
bool pdfDictionary::Has( const pdfObjId& id ) const throw()
{
	CheckIsValid(); 

	for (auto it = m_data->m_dictItems.begin(); it != m_data->m_dictItems.end(); ++it)
		if (it->second->m_id == id)
			return true;
	return false;
}

pdfString pdfDictionary::SetString( const char* key, const char* sVal )
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
#ifdef _MSC_VER
	pdfString newObj( doc.CreateString(sVal, StringEncoding::WinAnsiEncoding, false));
#else
	pdfString newObj( doc.CreateString(sVal, StringEncoding::ASCII, false));
#endif
	Set( key, newObj );
	return newObj;
}
pdfString pdfDictionary::SetString( const char* key, const char* sVal,  StringEncoding enc )
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfString newObj( doc.CreateString(sVal, enc, false));
	Set( key, newObj );
	return newObj;
}
pdfName pdfDictionary::SetName(const char* key, const char* sVal)
{
	CheckIsValid(); 
	if (key == nullptr || *key == 0 )
		throw pdfBlankKeyException();
	if (sVal == nullptr || *sVal == 0 )
		throw pdfBlankNameException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfName newObj( doc.CreateName(sVal, false));
	Set( key, newObj );
	return newObj;
}
pdfInt pdfDictionary::SetNumber(const char* key, int Val)
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfInt newObj( doc.CreateInteger(Val,false));
	Set( key, newObj );
	return newObj;
}
pdfInt pdfDictionary::SetNumber(const char* key, long Val)
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfInt newObj(  doc.CreateInteger(Val,false) );
	Set( key, newObj );
	return newObj;
}
pdfInt64 pdfDictionary::SetNumber(const char* key, int64 Val)
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfInt64 newObj(  doc.CreateInteger64(Val,false) );
	Set( key, newObj );
	return newObj;
}
pdfReal pdfDictionary::SetNumber( const char* key, float Val, int iPrecision )
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfReal newObj( doc.CreateReal(Val, iPrecision, false));
	Set( key, newObj );
	return newObj;
}
pdfReal pdfDictionary::SetNumber( const char* key, double Val, int iPrecision )
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfReal newObj(  doc.CreateReal(Val, iPrecision, false));
	Set( key, newObj );
	return newObj;
}
pdfBool pdfDictionary::SetBool( const char* key, bool Val )
{
	CheckIsValid(); 
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();

	pdfDocument doc( m_data->m_doc.lock() );
	pdfBool newObj( doc.CreateBoolean(Val, false) );
	Set( key, newObj );
	return newObj;
}
/*pdfAtom pdfDictionary::SetDict( const char* key, const pdfDictionary& Val )
{
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();
	return Set( key, pdfAtom(Val) );
}
pdfAtom pdfDictionary::SetArray( const char* key, const pdfArray& Val )
{
	if ( key == nullptr || *key == 0 )
		throw pdfBlankKeyException();
	return Set( key, pdfAtom(Val) );
}*/

size_t pdfDictionary::GetCount() const 
{
	if( m_data == nullptr  )
		return 0L; 
	return m_data->m_dictItems.size();
}

void pdfDictionary::Remove( const char* key )
{
	if ( Has( key ) )
	{
		auto iter = m_data->m_dictItems.find(key);
		if( iter != m_data->m_dictItems.end() )
		{
			pdfAtom atm;
			atm.m_data = iter->second;
			if ( !atm.IsNull() )
			{
				atm.Release();

				if ( !atm.IsIndirect()/* && !( atm is pdfReference )*/ )
				{
					atm.SetParent( pdfAtom() );
					atm.SetKey( nullptr );
				}
			}

			m_data->m_dictItems.erase( iter );
		}
	}
}
/*
void pdfDictionary::Remove( const size_t idx )
{
	CheckIsValid(); 
	//m_data->m_dictItems[szKey] = atm;
}


void pdfDictionary::Clear()
{
	if( m_data!=nullptr )
		m_data->m_dictItems.clear(); 
}
*/

//#include <typeinfo.h>

void pdfDictionary::TakeOwnership(const pdfAtom& atm, const char* szKey)
{
	// when storing make sure we use the reference object if it's an indirect object
	pdfAtom atmNew = atm;
	if ( atm.IsIndirect() )
		atmNew = atm.GetRef();


	// TODO Can we do this without RTTI?  (/GR)
	// Code size seems OK.

//	const std::type_info& t = typeid(atmNew) ;
//	string ss = t.name();
//	bool b = typeid(atm) == typeid(pdfReference);
	//bool bb = typeid(atm) == typeid(pdfDictionary);
	if ( !atmNew.IsIndirect() && atmNew.GetType() != Reference )//&& typeid(atmNew) != typeid(pdfReference) ) )
	{
		if ( !atmNew.GetParent().IsNull() )
			throw pdfAlreadyChildException();
		/// Set the "parent", i.e. Dictionary/Stream or Array, that contains this val.  Only for direct objects and 
		/// only when trying to determine the ID/Gen number for purposes of encryption.  Set when object is added to a Dictionary/Stream or Array.
		atmNew.SetParent( *this );
		atmNew.SetKey( szKey );
	}
	//
	// remove old
	Remove( szKey );
	atmNew.AddRef();
	m_data->m_dictItems[szKey] = atmNew.m_data;
}

//template< class _Fn1> 
//_Fn1 pdfDictionary::ForEach(_Fn1 _Func)
//{
//	if( m_data != nullptr )
//	{
//		// perform function for each element
//		for( auto it = m_data->m_dictItems.begin(); it!=m_data->m_dictItems.end(); ++it )
//		{
//			pdfAtom atm;
//			atm.m_data = *(it->second.get());
//			_Func( it->first.c_str(), atm );
//			//_Func( *(it->second.get()) );
//		}
//	}
//	return (_Func);
//}

};// namespace Exception
