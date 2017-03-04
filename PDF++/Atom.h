#pragma once
#include "ExportDef.h"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "Enum.h"
#include "ObjId.h"
#include "AtomData.h"
#include "Serializable.h"
#include "pdfException.h"
#include <functional>

namespace Pdf{

class pdfDocument;

class PDF_API pdfAtom : public CSerializable
{
	friend class pdfReference;
	friend class pdfReader;
	friend class pdfDictionary;
	//friend class pdfArray;
	//friend std::ostream& operator<<( std::ostream& stm, const pdfAtom& atm );
public:
	pdfAtom(void);
protected:
	// used internally when building from the ground up - usually via the ctor of a derived type
	// their versions of this constructor are public since a type is know at the start.
	pdfAtom(const pdfDocument& doc);// TODO Internal only?
	pdfAtom(const std::shared_ptr<CDocData>& src);  // TODO enable this and for all derived instead of or in addition to pdfDocument
public:
	pdfAtom(const pdfAtom& atm);
	pdfAtom(pdfAtom&& atm);
	virtual ~pdfAtom(void);

	pdfAtom& operator=(const pdfAtom& i);
	pdfAtom& operator=(pdfAtom&& atm);

	inline bool operator!() const { return IsNull(); };
	inline operator bool() const { return !IsNull(); };

	inline bool operator<( const pdfAtom& rhs ) const { return Compare(rhs) < 0; };
	inline bool operator<=( const pdfAtom& rhs ) const { return Compare(rhs) <= 0; };
	inline bool operator==( const pdfAtom& rhs ) const { return Compare(rhs) == 0; };
	inline bool operator!=( const pdfAtom& rhs ) const { return Compare(rhs) != 0; };

	inline bool operator<( const pdfObjId& rhs ) const { return Compare(rhs) < 0; };
	inline bool operator<=( const pdfObjId& rhs ) const { return Compare(rhs) <= 0; };
	inline bool operator==( const pdfObjId& rhs ) const { return Compare(rhs) == 0; };
	inline bool operator!=( const pdfObjId& rhs ) const { return Compare(rhs) != 0; };

	// makes it so the caller can do:  
	// pdfAtom atm;
	// ...
	// atm = nullptr;  <-----
	inline pdfAtom& operator=(const std::nullptr_t i)// specifically for clearing out the data and being set to a Null state
		{ SetValue( i );return *this; };
	inline bool operator==( const std::nullptr_t rhs ) const { return IsNull(); };
	inline bool operator!=( const std::nullptr_t rhs ) const { return !IsNull(); };

	// CSerializable
	void Serialize( std::ostream& stm );

	virtual int Compare(const pdfAtom& rhs) const;
	virtual int Compare(const pdfObjId& rhs) const;

	// use when this is a reference object to get the actual object - returns *this if this isn't a reference
	pdfAtom Dereference() const;

	pdfDocument GetDocument() const;
	inline AtomType GetType() const throw()
		{
			if( m_data==nullptr )
				return Null;
			return m_data->m_type;
		};
	inline pdfObjId GetId() const
		{
			if( m_data==nullptr )
				throw Pdf::Exception::pdfDocUndefinedException();
			return m_data->m_id;
		};
	inline void SetId(const pdfObjId& newId)
		{
			if( m_data==nullptr )
				throw Pdf::Exception::pdfDocUndefinedException();
			m_data->m_id = newId;
		};

	/// <summary>
	/// Indicates whether the string or stream data is in an encrypted state.
	/// </summary>
	inline bool GetEncrypted() const throw()
		{
			if( m_data==nullptr  )
				return false;
			return m_data->m_bEncrypted;
		};

	inline void SetEncrypted(bool b)
		{
			if( m_data==nullptr )
				throw Pdf::Exception::pdfDocUndefinedException();
			m_data->m_bEncrypted = b;
		};

	inline const char* GetKey() const
	{
		if( m_data==nullptr )
			return nullptr;//throw pdfDocUndefinedException();
		return m_data->m_sKey.c_str();
	}
	inline void SetKey(const char* szNew)
	{
		if( m_data==nullptr )
			throw Pdf::Exception::pdfDocUndefinedException();
		m_data->m_sKey = szNew == nullptr ? "" : szNew  ;
	}

	inline bool IsInteger() const throw()
	{
		if( m_data==nullptr  )
			return false;//throw pdfDocUndefinedException();
		return ( m_data->m_type == Integer || m_data->m_type == Integer64 );
	}

	inline bool IsNumber() const throw()
	{
		if( m_data==nullptr  )
			return false;//throw pdfDocUndefinedException();
		return ( m_data->m_type == Integer || m_data->m_type == Integer64 || m_data->m_type == AtomType::Real );
	}

	inline bool IsDictionary() const throw()
	{
		if( m_data==nullptr  )
			return false;//throw pdfDocUndefinedException();
		return ( m_data->m_type == Dictionary || m_data->m_type == Stream );
	}

	inline bool IsStream() const throw()
	{
		if( m_data==nullptr  )
			return false;//throw pdfDocUndefinedException();
		return ( m_data->m_type == Stream );
	}

	inline bool IsArray() const throw()
	{
		if( m_data==nullptr  )
			return false;//throw pdfDocUndefinedException();
		return ( m_data->m_type == Array );
	}

	inline bool IsNull() const throw()
	{
		if( m_data==nullptr  )
			return true;
		return ( m_data->m_type == Null );
	}

	inline bool IsUndefined() const throw()
	{
		return ( m_data!=nullptr && m_data->m_type == Null && !m_data->m_doc.expired() );
	}

	inline bool IsIndirect() const throw()
	{
		if( m_data==nullptr  )
			return true;
		return ( m_data->m_ref != nullptr );
	}

	inline bool Bool() const
	{
		bool i = 0;
		GetValue( i );
		return i;
	}
	inline  int Int()  const
	{
		int i = 0;
		GetValue( i );
		return i;
	}
	inline int64 Int64() const
	{
		int64 i = 0L;
		GetValue( i );
		return i;
	}
	inline double Real() const
	{
		double i = 0;
		GetValue( i );
		return i;
	}
	inline float RealF() const
	{
		float i = 0;
		GetValue( i );
		return i;
	}
	inline std::string Str()
	{
		std::string i;
		GetValue( i );
		return i;
	}

	void GetValue( int& i ) const;
	void GetValue( int64& i ) const;
	void GetValue( float& i ) const;
	void GetValue( double& i ) const;
	void GetValue( std::string& i );
	void GetValue( std::wstring& i );
	void GetValue( bool& i ) const;

	// returns the reference obj that is pointing to this one.
	pdfAtom GetRef() const;
	void SetRef(const pdfAtom& src);

	pdfAtom GetParent() const;
	void SetParent(const pdfAtom& src);

	void SetValue( const int& i );
	void SetValue( const int64& i );
	void SetValue( const float& i );
	void SetValue( const double& i );
	void SetValue( const char* i );
	void SetValue( const wchar_t* i );
	void SetValue( const bool& i );
	void SetValue( const std::nullptr_t i );// specifically for clearing out the data and being set to a Null state

	virtual void CheckIsValid() const;// throw (Pdf::Exception::pdfDocUndefinedException); 

	// These helpers are used only by the collection objects (dictionary and array)
	// Used for references when we have to affect its reference count (like when adding to a collection)
	size_t AddRef();
	size_t Release();

	//const CAtomData::Ptr GetData() const throw();
	inline const CAtomData* GetData() const throw() { return m_data!=nullptr ? m_data.get() : nullptr; };

	CAtomData::Ptr m_data;
};

// for use with ForEach on the dictionary class
typedef std::function<bool( const char* key, pdfAtom& atm )> EnumDictAtom;
typedef std::function<void( const char* key, pdfAtom& atm )> EnumDictAtom2;

// for use with ForEach on the dictionary and array classes
typedef std::function<bool( pdfAtom& atm )> EnumArrayAtom;
typedef std::function<void( pdfAtom& atm )> EnumArrayAtom2;

} ; //namespace Pdf

// specifically for clearing out the data and being set to a Null state
//template<typename T>
//PDF_API T& operator=(T&, const std::nullptr_t i)
//{
//	T.SetValue( i );
//	return T;
//};
//PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::pdfAtom& atm );
