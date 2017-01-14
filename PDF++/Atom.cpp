#include "stdafx.h"
#include "Atom.h"
#include "pdfException.h"
#include "pdfReader.h"
#include "Document.h"

#ifdef __GNUC__
#include <bits/codecvt.h>
//#include <cvt\utf8>
//#include <cvt\wstring>
#else
#include <cvt\utf8>
#include <cvt\wstring>
#include <codecvt>
#endif

using namespace Pdf::Exception;
/*
template<typename T>
void operator=(T&, const std::nullptr_t i)// specifically for clearing out the data and being set to a Null state
{
	T.SetValue( i );
	return *this;
};
*/
namespace Pdf{

pdfAtom::pdfAtom(void)
{
	m_data.reset( new CAtomData() );
}

pdfAtom::pdfAtom(const pdfAtom& src)
{
	m_data = src.m_data;
	if( m_data==nullptr )
		m_data.reset( new CAtomData() );
}

pdfAtom::pdfAtom(pdfAtom&& atm)
{//not sure if this really helper considering we're using a shared_ptr
	m_data = atm.m_data;
	atm.m_data.reset( new CAtomData() );// = nullptr;
}

// used internally when building from the ground up - usually via the ctor of a derived type
pdfAtom::pdfAtom(const pdfDocument& doc)
{
	m_data.reset( new CAtomData(doc.m_data) );
}


// used internally when building from the ground up - usually via the ctor of a derived type
pdfAtom::pdfAtom(const std::shared_ptr<CDocData>& doc)
{
	m_data.reset( new CAtomData(doc) );
}

pdfAtom::~pdfAtom(void)
{
}

pdfAtom& pdfAtom::operator=(const pdfAtom& src)
{
	if( this != &src )
	{
		// hold the shared object until the op is complete
		// we don't want the = assignment to release first
		CAtomData::Ptr tmp ( m_data );

		m_data = src.m_data;
		if( m_data==nullptr )//|| m_data->m_doc == nullptr )
			m_data.reset( new CAtomData() );//throw pdfDocUndefinedException(); 
	}
	return *this;
}

pdfAtom& pdfAtom::operator=(pdfAtom&& atm)
{//not sure if this really helper considering we're using a shared_ptr
	if( this != &atm )
	{
		m_data = atm.m_data;
		atm.m_data = nullptr;
	}
	return *this;
}

pdfDocument pdfAtom::GetDocument() const 
{
	return pdfDocument( m_data->m_doc.lock() );
};

void pdfAtom::Serialize(  std::ostream& stm )
{
	if( m_data != nullptr )
	{
		//stm << m_data->m_id << ' ' << m_data->m_type ;
		switch( m_data->m_type )
		{
			case Null:
				stm << m_data->m_type ;
				break;
			case Integer:
				stm << m_data->m_type << " '" << Int() << "'" ;
				break;
			case Integer64:
				stm << m_data->m_type << " '" << Int64() << "'" ;
				break;
			case AtomType::Real:
				stm << m_data->m_type << " '" << Real() << "'" ;
				break;
			case Boolean:
				stm << m_data->m_type << " '" << Bool() << "'" ;
				break;
			case String:
				//stm << m_data->m_type << " '" << Str() << "'";
				//break;
			case Keyword:
				//stm << "Name" << " '" << Str() << "'";
				//break;
			case Name:
				stm << m_data->m_type << " '" << Str() << "'";
				break;
			case Dictionary:
				stm << m_data->m_type ;
				break;
			case Stream:
				{
					stm << m_data->m_type ;
					//if( m_data->m_stmBuff.GetLength() > 0 )
					{
						try
						{
							pdfBlob b(*this);
							obytestream stmBuff;
							b.GetBytes( (std::ostream&)stmBuff, true, true );
							stm << " " << stmBuff.GetLength() << " bytes: <";
							if( stmBuff.GetLength()>0 )
							{
								stm << std::hex ;

								byte* p = const_cast<byte*>(stmBuff.GetBuffer());
								size_t i = 0;
								while( i++ < stmBuff.GetLength() && i < 10 )
									stm << (int)*p++;
								if( i < stmBuff.GetLength() )
									stm << "...";
							}
							stm << ">" ;
						}
						catch(...)
						{
						}
						stm << std::dec;
					}
				}
				break;
			case Array:
				stm << m_data->m_type ;
				break;

			// only used internally.
			case Reference:
				stm << m_data->m_id << ' ' << m_data->m_type ;
				break;
		}
	}
}

void pdfAtom::CheckIsValid() const //throw (Pdf::Exception::pdfDocUndefinedException)
{
	if( m_data==nullptr || m_data->m_doc.expired() )//|| !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 
}

int pdfAtom::Compare(const pdfAtom& rhs) const
{
	if( IsNull() && rhs.IsNull() )
		return 0;
	if( IsNull() && !rhs.IsNull() )
		return 0xFFFF;
	if( !IsNull() && rhs.IsNull() )
		return 0x7f00FFFF;

	int i = m_data->m_id.Compare( rhs.GetId() );
	if( i == 0 )
	{
		//if( m_data.get() == rhs.m_data.get() )
			i = (m_data.get() - rhs.m_data.get());
	}

	return i;
}

int pdfAtom::Compare(const pdfObjId& rhs) const//not useful if this is a direct object - i.e., "0 0 obj", because there could be many
{
	if( IsNull()  )
		return -1;
	return m_data->m_id.Compare( rhs );
}

// use when this is a reference object to get the actual object - returns *this if this isn't a reference
pdfAtom pdfAtom::Dereference() const
{
	if( IsNull() )
		return *this;

	CheckIsValid();

	pdfAtom atm;
	if( m_data->m_type == AtomType::Reference && !m_data->m_id.IsFree() )
	{
		pdfDocument doc( m_data->m_doc.lock() );
		atm.m_data = m_data->m_RefTarget;

		if( atm.IsNull() )
		{
			pdfReference refFound;

			// been loaded in from the file yet? Not if the position is set to MinValueInt64
			if (m_data->m_FilePos == MinValueInt64 )
			{ 
				refFound = doc.CrossReference()[ m_data->m_id ];//pdfAtom newVal = Document.FindReferencedObj(m_data->m_id);
				if (!refFound.IsNull())
				{
					//refFound = newVal as pdfReference;
					m_data->m_FilePos = refFound.m_data->m_FilePos;
					m_data->m_RefTarget = refFound.m_data->m_RefTarget;
					if (m_data->m_RefTarget != nullptr)
					{
						atm.m_data = m_data->m_RefTarget;
						return atm;
					}
				}
				else
				{
					char szMsg[50] = "";
#ifdef _WINDOWS
					sprintf_s( szMsg, sizeof(szMsg)/sizeof(szMsg[0]), "Id %s not found", GetId().ToString().c_str() ); 
#else
					sprintf( szMsg, "Id %s not found", GetId().ToString().c_str() );
#endif
					throw pdfException( szMsg );
				}
			}

			pdfReader& parser( *(m_data->m_doc.lock()->m_Parser) );
			int64 iCur = parser.GetCursor();
			parser.SetCursor( m_data->m_FilePos );

			try
			{
				// skip the object marker:  id, gen and "obj" --->   1 0 obj
				pdfAtom atm1 = parser.ReadObj();
				pdfAtom atm2 = parser.ReadObj();
				pdfAtom atm3 = parser.ReadObj();

				//read the actual obejct
				m_data->m_RefTarget = parser.ReadObj().m_data;
			}
			catch(...)
			{
				parser.SetCursor( iCur );
				throw;
			}
			parser.SetCursor( iCur );

			// set some relationships
			if( m_data->m_RefTarget != nullptr )
				m_data->m_RefTarget->m_ref = m_data;

			//
			if (!refFound.IsNull())
			{
				refFound.m_data->m_RefTarget = m_data->m_RefTarget;
				if (refFound.m_data->m_RefTarget != nullptr)
					refFound.m_data->m_RefTarget->m_ref = refFound.m_data;//refFound.m_data->m_RefTarget.SetRef( refFound );
			}

			//fill the output object
			atm.m_data = m_data->m_RefTarget;
		}


		//_ASSERT( !m_data->m_id.IsFree() && !atm.IsNull() );
		return atm; 
	}
	return *this;
}

pdfAtom pdfAtom::GetRef() const 
{
	CheckIsValid();;

	pdfAtom atm;
	atm.m_data = m_data->m_ref;
	return atm; 
}

void pdfAtom::SetRef(const pdfAtom& src)
{
	CheckIsValid();;

	if( src.m_data!=nullptr )
	{
		if( src==*this)
			throw pdfBadRelationshipException();
		
		m_data->m_ref = src.m_data;
		if( m_data->m_ref!=nullptr )
			m_data->m_id = src.GetId();
	}
	else
	{
		m_data->m_ref = nullptr;
		//if( m_data->m_ref!=nullptr )
		//	m_data->m_id = src->m_data->m_id;
	}
};

pdfAtom pdfAtom::GetParent() const 
{
	CheckIsValid();;

	pdfAtom atm;
	if( !m_data->m_pParent.expired() )//&& m_data->m_pParent._Get() !=nullptr  )
		atm.m_data = m_data->m_pParent.lock();
	return atm; 
}

void pdfAtom::SetParent(const pdfAtom& src)
{
	CheckIsValid();;

	pdfAtom pOldParent;
	if( !src.IsNull() )
		pOldParent = src.GetParent();
	if( src==*this || ( !pOldParent.IsNull() && pOldParent ==*this ) )
		throw pdfBadRelationshipException();

	if( src.m_data!=nullptr && src.m_data->m_type != AtomType::Null )
		m_data->m_pParent = src.m_data;
	else
		m_data->m_pParent.reset();
}


void pdfAtom::GetValue( int& i ) const
{
	CheckIsValid();

		pdfAtom atmThis( Dereference() );
	switch( atmThis.m_data->m_type )
	{
	case Integer :
		i = atmThis.m_data->m_Val.iVal;
		break;
	case Integer64 :
		i = atmThis.m_data->m_Val.i64Val;
		break;
	case AtomType::Real :
		i = atmThis.m_data->m_Val.dVal;
		break;
	case Boolean :
		i = atmThis.m_data->m_Val.bVal;
		break;
	default :
		throw pdfTypeMismatchException();
	}
}
void pdfAtom::GetValue( int64& i ) const
{
	CheckIsValid();

	pdfAtom atmThis( Dereference() );
	switch( atmThis.m_data->m_type )
	{
	case Integer :
		i = atmThis.m_data->m_Val.iVal;
		break;
	case Integer64 :
		i = atmThis.m_data->m_Val.i64Val;
		break;
	case AtomType::Real :
		i = atmThis.m_data->m_Val.dVal;
		break;
	case Boolean :
		i = atmThis.m_data->m_Val.bVal;
		break;
	default :
		throw pdfTypeMismatchException();
	}
}
void pdfAtom::GetValue( float& i ) const
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	switch( atmThis.m_data->m_type )
	{
	case Integer :
		i = atmThis.m_data->m_Val.iVal;
		break;
	case Integer64 :
		i = atmThis.m_data->m_Val.i64Val;
		break;
	case AtomType::Real :
		i = atmThis.m_data->m_Val.dVal;
		break;
	case Boolean :
		i = atmThis.m_data->m_Val.bVal;
		break;
	default :
		throw pdfTypeMismatchException();
	}
}
void pdfAtom::GetValue( double& i ) const
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	switch( atmThis.m_data->m_type )
	{
	case Integer :
		i = atmThis.m_data->m_Val.iVal;
		break;
	case Integer64 :
		i = atmThis.m_data->m_Val.i64Val;
		break;
	case AtomType::Real :
		i = atmThis.m_data->m_Val.dVal;
		break;
	case Boolean :
		i = atmThis.m_data->m_Val.bVal;
		break;
	default :
		throw pdfTypeMismatchException();
	}
}

void pdfAtom::GetValue( std::string& sVal )
{
	CheckIsValid();
 

	pdfDocument doc( m_data->m_doc.lock() );
	pdfAtom atmThis( Dereference() );

	if( atmThis.m_data->m_type != String && atmThis.m_data->m_type != Keyword && atmThis.m_data->m_type != Name )
		throw pdfTypeMismatchException();

	// decrypt?
	if( atmThis.m_data->m_type == String && atmThis.m_data->m_bEncrypted && doc.IsEncrypted() )
	{
		pdfString str(atmThis);
		str.GetStringBytes(true);//will decrypt
	}

	//	size_t x = sizeof(CAtomData);
	//	size_t z = sizeof(pdfAtom);
	if (atmThis.m_data->m_hexData.GetLength() > 0 )// wouldn't be a Name in this case, must be a String
	{
		switch (atmThis.m_data->m_StringEncoding)
		{
			case StringEncoding::ASCII:
			{
				//sVal = Encoding.ASCII.GetString(atmThis.m_data->m_hexData.get(), 0, atmThis.m_data->m_hexData.GetLength());
				/*std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
				char* pFirst = (char*)atmThis.m_data->m_hexData.get();
				char* pLast = pFirst + atmThis.m_data->m_hexData.GetLength();
				std::u16string dest = convert.from_bytes( pFirst, pLast ); */
				sVal.assign( (char*)atmThis.m_data->m_hexData.get(),atmThis.m_data->m_hexData.GetLength() );
			}
			break;
			/*case StringEncoding::UTF16BE:
				sVal = Encoding.BigEndianUnicode.GetString(atmThis.m_data->m_hexData.get(), 0, atmThis.m_data->m_hexData.GetLength());
				break;
			case StringEncoding::UTF16LE:
				sVal = Encoding.Unicode.GetString(atmThis.m_data->m_hexData.get(), 0, atmThis.m_data->m_hexData.GetLength());
				break;
			case StringEncoding::MacRomanEncoding:
				sVal = pdfEncoding.MacRoman.GetString(atmThis.m_data->m_hexData.get(), 0, atmThis.m_data->m_hexData.GetLength());
				break;
			case StringEncoding::PDFDocEncoding:
				sVal = pdfEncoding.PDF.GetString(atmThis.m_data->m_hexData.get(), 0, atmThis.m_data->m_hexData.GetLength());
				break;
			case StringEncoding::StdEncoding:
				sVal = pdfEncoding.Std.GetString(atmThis.m_data->m_hexData.get(), 0, atmThis.m_data->m_hexData.GetLength());
				break;
			case StringEncoding::WinAnsiEncoding:
				sVal = pdfEncoding.WinAnsi.GetString(atmThis.m_data->m_hexData.get(), 0, atmThis.m_data->m_hexData.GetLength());
				break;*/
		}
	}
	else
		sVal = atmThis.m_data->m_strName;

	/*
	std::string source = "Hi there";
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
	std::u16string dest = convert.from_bytes(source); 

	*/
}
void pdfAtom::GetValue( std::wstring& i )
{
	throw pdfNotImplException(); 
}
void pdfAtom::GetValue( bool& i ) const
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	switch( atmThis.m_data->m_type )
	{
	case Integer :
		i = atmThis.m_data->m_Val.iVal!=0;
		break;
	case Integer64 :
		i = atmThis.m_data->m_Val.i64Val!=0;
		break;
	case AtomType::Real :
		i = atmThis.m_data->m_Val.dVal!=0.0;
		break;
	case Boolean :
		i = atmThis.m_data->m_Val.bVal;
		break;
	default :
		throw pdfTypeMismatchException();
	}
}
/*

void pdfAtom::SetValue( const int& i )
{ operator=(i); }
void pdfAtom::SetValue( const int64& i )
{ operator=(i); }
void pdfAtom::SetValue( const float& i )
{ operator=(i); }
void pdfAtom::SetValue( const double& i )
{ operator=(i); }
void pdfAtom::SetValue( const char* i )
{ operator=(i); }
void pdfAtom::SetValue( const wchar_t* i )
{ operator=(i); }
void pdfAtom::SetValue( const bool& i )
{ operator=(i); }
void pdfAtom::SetValue( const std::nullptr_t i )
{ operator=(i); }
*/

void pdfAtom::SetValue(const std::nullptr_t i)
{
	m_data.reset( new CAtomData() );

}
void pdfAtom::SetValue(const int& i)
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	atmThis.m_data->m_type = Integer;
	atmThis.m_data->m_Val.iVal = i;

}
void pdfAtom::SetValue(const int64& i)
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	atmThis.m_data->m_type = Integer64;
	atmThis.m_data->m_Val.i64Val = i;

}
void pdfAtom::SetValue(const float& i)
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	atmThis.m_data->m_type = AtomType::Real;
	atmThis.m_data->m_Val.dVal = i;

}
void pdfAtom::SetValue(const double& i)
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	atmThis.m_data->m_type = AtomType::Real;
	atmThis.m_data->m_Val.dVal = i;

}
void pdfAtom::SetValue(const char* i)
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
#ifdef _WINDOWS
	atmThis.m_data = pdfString( pdfDocument( atmThis.m_data->m_doc.lock() ), i, WinAnsiEncoding ).m_data;
#else
	pdfDocument doc( atmThis.m_data->m_doc.lock() );
	atmThis.m_data = pdfString( doc, i, ASCII ).m_data;
#endif
	//atmThis.m_data->m_type = String;
	//atmThis.m_data->m_strVal = i;

}
void pdfAtom::SetValue(const wchar_t* i)
{
	CheckIsValid();
	pdfAtom atmThis( Dereference() );
	pdfDocument doc( m_data->m_doc.lock() );
#ifdef _WINDOWS
	atmThis.m_data = pdfString( doc, i, WinAnsiEncoding ).m_data;
#else
	atmThis.m_data = pdfString( doc, i, ASCII ).m_data;
#endif
}

void pdfAtom::SetValue(const bool& i)
{
	CheckIsValid();
 
	pdfAtom atmThis( Dereference() );
	atmThis.m_data->m_type = Boolean;
	atmThis.m_data->m_Val.bVal = i;

}
// These helpers are used only by the collection objects (dictionary and array)
// Used for references when we have to affect its reference count (like when adding to a collection)
size_t pdfAtom::AddRef()
{
	pdfAtom atmNew(*this);
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
size_t pdfAtom::Release()
{
	pdfAtom atmNew(*this);
	if ( atmNew.IsIndirect() )
		atmNew = atmNew.GetRef();
	if ( atmNew.GetType() == Reference )
	{
		pdfReference refer(atmNew);
		refer.DecrementCount();
		return refer.GetReferenceCount();
	}
	return 1;
}


} ; //namespace Pdf

////////////////////////  OUTSIDE of namespace

/*std::ostream& operator<<( std::ostream& stm, const Pdf::pdfAtom& atm )
{
	if( atm.GetData() != nullptr )
	{
		stm << atm.GetData()->m_id << ' ' << atm.GetType();
		//switch( atm.GetType() )
		//{
		//}
	}
	return stm;
};*/
