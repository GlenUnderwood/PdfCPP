#include "stdafx.h"
#include "String.h"
#include "Enum.h"
#include "Document.h"
#include "pdfReader.h"

namespace Pdf {

pdfString::pdfString(void) ////: pdfAtom()
{
	m_data->m_type = AtomType::String;
}

pdfString::pdfString(const pdfAtom& src) : pdfAtom(src)
{
	AtomType t = src.GetType();
	if( t != AtomType::Null && t != AtomType::String && t != AtomType::Name && t != AtomType::Keyword )
		throw pdfTypeMismatchException();

	//pdfAtom::operator=(src);
}

pdfString::pdfString(pdfDocument& doc) : pdfAtom(doc)
{
	m_data->m_type = AtomType::String;
	//m_data->m_StringEncoding = enc;
}

pdfString::pdfString(pdfDocument& doc, const char* i, StringEncoding enc) : pdfAtom(doc)
{
//	pdfAtom::operator=(i);
	m_data->m_type = String;
	//m_data->m_strVal = i;
	//m_data->m_StringEncoding = enc;
	SetStringBytes( (const byte*)i, strlen(i), enc );
}

pdfString::pdfString(pdfDocument& doc, const wchar_t* i, StringEncoding enc) : pdfAtom(doc)
{
	// TODO Allow only Unicode BE or LE
	throw pdfNotImplException();
	//pdfAtom::operator=(i);
	m_data->m_type = String;
	m_data->m_StringEncoding = enc;
}

pdfString::pdfString(pdfDocument& doc, const byte* pBytes, size_t nBytes, StringEncoding enc /*, bool bTakeOwnership*/) : pdfAtom(doc)
{
	m_data->m_type = String;

	SetStringBytes( pBytes, nBytes, enc );
}

pdfString::pdfString(pdfDocument& doc, const CSharedBuff& src, StringEncoding enc /*, bool bTakeOwnership*/) : pdfAtom(doc)
{
	m_data->m_type = String;

	SetStringBytes( src, enc );
}

pdfString::~pdfString(void)
{
}

pdfString& pdfString::operator=(const pdfAtom& src)
{
	if( this != &src ) 
	{
		AtomType t = src.GetType();
		if( t != AtomType::String && t != AtomType::Name && t != AtomType::Keyword )
			throw pdfTypeMismatchException();

		pdfAtom::operator=(src);
	}
	return *this;
}

StringEncoding pdfString::GetStringEncoding() const
{
	if( m_data==nullptr || m_data->m_doc.expired() )
		throw pdfDocUndefinedException(); 
	return m_data->m_StringEncoding;
}

void pdfString::SetStringEncoding( StringEncoding enc )
{
	if( m_data==nullptr || m_data->m_doc.expired() )
		throw pdfDocUndefinedException(); 
	m_data->m_StringEncoding = enc;
}

CSharedBuff pdfString::GetStringBytes(bool bDecrypt) const
{
	if( m_data==nullptr || m_data->m_doc.expired() )
		throw pdfDocUndefinedException(); 

	pdfDocument doc( m_data->m_doc.lock() );

	if ( bDecrypt && m_data->m_bEncrypted && doc.IsEncrypted() )
	{
		m_data->m_bEncrypted = !doc.GetSecurity()->DecryptObject( *this, m_data->m_hexData );

		if ( !m_data->m_bEncrypted && m_data->m_hexData.GetLength() > 0 )
		{
			m_data->m_StringEncoding = StringEncoding::ASCII;

			// here I create a temporary stream to parse the bytes with the reader as we would if radig from a file.
			// ths way the rules for skipping escape chars, etc. remain intact

			// The parser assumes anything from the file is encrypted so undo this, since we already decrypted it unflip this flag 
			// so it goes down the normal route.
			m_data->m_doc.lock()->m_bEncrypted = false;

			try
			{
				/*CSharedBuff hexData( m_data->m_hexData );// just to make sure a reference remains for the life this scope
				
				// this type will just use the memory location, doesn't alloc/free - read-only!
				sharedbytebuf tmpBuf( hexData.get(), hexData.GetLength() );//std::ios_base::in );
				ibytestream memStream;//( std::ios_base::in | std::ios_base::out );
				// tell the stream to use tmpBuf as the buffer.
				memStream.rdbuf(&tmpBuf);*/

				pdfReader parser( m_data->m_doc.lock(), m_data->m_hexData);
				//parser.Reset( (std::istream*)&memStream );

				pdfAtom atm = parser.ReadString();

				// attach to the results.
				if ( !atm.IsNull() )
				{
					m_data->m_hexData = atm.GetData()->m_hexData;
					m_data->m_StringEncoding = atm.GetData()->m_StringEncoding;
				}

				// put the read point back to the beginning after using this as a stream.
				istream& stm( m_data->m_hexData );
				size_t i = stm.tellg();// TODO see why this isn't returning > 0 - i.e. position still at beg
				stm.seekg( ios::beg );
			}
			catch ( const std::exception& ex )
			{
				// TODO do something!
			}
			m_data->m_doc.lock()->m_bEncrypted = true;
		}
	}

	return m_data->m_hexData;
}

void pdfString::SetStringBytes(const CSharedBuff& src, StringEncoding enc  )
{
	if( m_data==nullptr || m_data->m_doc.expired() )
		throw pdfDocUndefinedException(); 
	m_data->m_StringEncoding = enc;
	m_data->m_hexData.Clear();
	m_data->m_hexData = src;
}

void pdfString::SetStringBytes(const byte* pBytes, size_t nBytes, StringEncoding enc  )
{
	if( m_data==nullptr || m_data->m_doc.expired() )
		throw pdfDocUndefinedException(); 

	m_data->m_StringEncoding = enc;
	m_data->m_hexData.Clear();
	if( pBytes && nBytes > 0 )
	{
		m_data->m_hexData.Resize( nBytes,false);
		memcpy( m_data->m_hexData.get(), pBytes, nBytes );
	}
}

};// namespace Exception
