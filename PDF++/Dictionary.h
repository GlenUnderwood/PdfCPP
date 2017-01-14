#pragma once
#include <memory>
#include "Atom.h"
#include "Int.h"
#include "Int64.h"
#include "Real.h"
#include "Bool.h"
#include "String.h"
#include "Name.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfDictionary : public pdfAtom
{
public:
	pdfDictionary(void);
	pdfDictionary(pdfDocument& doc);
	pdfDictionary(const pdfAtom& src);
	virtual ~pdfDictionary(void);

	pdfDictionary& operator=(const pdfAtom& src);

	inline pdfDictionary& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

	pdfAtom operator[]( const size_t  i )  const;
	pdfAtom operator[]( const char* i );// const;
	pdfAtom operator[]( const pdfObjId& i );// const;
	pdfAtom operator[]( const std::string& i );// const;

	//void Add( const char* szKey, const pdfAtom& atm ); 
	bool Has( const char* szKey ) const throw();
	bool Has( const pdfObjId& id ) const throw();

	pdfAtom Get( const size_t  i ) const;
	pdfAtom Get( const char* szKey ) const;
	pdfAtom Get( const pdfObjId& id ) const;
	pdfAtom Get( const std::string& szKey ) const;

	pdfAtom Set( const size_t i, const pdfAtom& atm );
	pdfAtom Set( const char* szKey, const pdfAtom& atm );
	inline pdfAtom Set( const std::string& szKey, const pdfAtom& atm )
	{
		return Set( szKey.c_str(), atm );
	}

	inline pdfAtom Set( const char* key, const char* Val,  StringEncoding enc )
		{ return SetString( key, Val, enc ); };
	inline pdfAtom Set( const char* key, const char* Val )// For Name types
		{ return SetName( key, Val ); };
	inline pdfAtom Set( const char* key, int Val )
		{ return SetNumber( key, Val ); };
	inline pdfAtom Set( const char* key, long Val )
		{ return SetNumber( key, Val ); };
	inline pdfAtom Set( const char* key, int64 Val )
		{ return SetNumber( key, Val ); };
	inline pdfAtom Set( const char* key, float Val, int iPrecision = 6 )
		{ return SetNumber( key, Val, iPrecision ); };
	inline pdfAtom Set( const char* key, double Val, int iPrecision = 6 )
		{ return SetNumber( key, Val, iPrecision ); };
	inline pdfAtom Set( const char* key, bool Val )
		{ return SetBool( key, Val ); };

	pdfString SetString( const char* key, const char* sVal );
	pdfString SetString( const char* key, const char* sVal,  StringEncoding enc );
	pdfName SetName( const char* key, const char* sVal );
	pdfInt SetNumber( const char* key, int Val );
	pdfInt SetNumber( const char* key, long Val );
	pdfInt64 SetNumber( const char* key, int64 Val );
	pdfReal SetNumber( const char* key, float Val, int iPrecision = 6 );
	pdfReal SetNumber( const char* key, double Val, int iPrecision = 6 );
	pdfBool SetBool( const char* key, bool Val );

	size_t GetCount() const ;
	void Remove( const size_t idx );
	void Remove( const char* szKey );
	//void Clear();

	template< class _Fn1> inline
		_Fn1 ForEach( _Fn1 _Func )
	{
		if (m_data != nullptr)
		{
			// perform function for each element
			for (auto it = m_data->m_dictItems.begin(); it != m_data->m_dictItems.end(); ++it)
			{
				pdfAtom atm;
				atm.m_data = it->second;
				atm = atm.Dereference();
				_Func( it->first.c_str(), atm );
				//_Func( *(it->second.get()) );
			}
		}
		return (_Func);
	}
	/*
	{	// perform function for each element
		for (; _First != _Last; ++_First)
			_Func(*_First);
		return (_Func);
	}*/
	template<typename T>
	T _Get( const char* szKey, T defVal  )
	{
		T bResult = defVal;
		pdfAtom b( Get(szKey) );
		if (b)
			b.GetValue(bResult);
		return bResult;
	}
	template<typename T>
	void _Set(const char* szKey, const T& value)
	{
		if (Has(szKey))
		{
			pdfAtom b( Get(szKey) );
			if (b)
				b.SetValue( value );
		}
		else
			Set( szKey, value );
	}

protected:
	void TakeOwnership(const pdfAtom& atm, const char* key);


};

};// namespace Pdf
