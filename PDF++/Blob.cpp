#include "stdafx.h"
#include "Blob.h"
#include "Enum.h"
#include "Document.h"
#include "FlateFilter.h"
#include "pdfReader.h"

namespace Pdf {

pdfBlob::pdfBlob(void) : pdfDictionary()
{
	//m_data->m_type = AtomType::Stream;
}

pdfBlob::pdfBlob(const pdfAtom& atm ) : pdfDictionary(atm)
{
	m_data->m_type = AtomType::Stream;
}

pdfBlob::pdfBlob(pdfDocument& doc ) : pdfDictionary(doc)
{
	m_data->m_type = AtomType::Stream;
	m_data->m_FilePos = MinValueInt64;
}

pdfBlob::pdfBlob(const pdfDictionary& src, const size_t iPosition ) : pdfDictionary(src)
{
	m_data->m_type = AtomType::Stream;
	m_data->m_FilePos = iPosition;//>m_iStreamPos 
}

pdfBlob::~pdfBlob(void)
{
}

pdfBlob& pdfBlob::operator=(const pdfAtom& src )
{
	if( this != &src )
	{
		AtomType t = src.GetType();
		if( t != AtomType::Null && t != AtomType::Dictionary && t != AtomType::Stream )
			throw pdfTypeMismatchException();

		m_data->m_type = AtomType::Stream;

		pdfAtom::operator=(src);
	}
	return *this;
}

bool pdfBlob::NeedsLoading() const
{
	return ( m_data != nullptr && m_data->m_stmBuff.GetLength() == 0 && m_data->m_FilePos > 0 && Has( "Length" ) );
};

/*bool pdfBlob::HasData()
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		return false; 

	if ( NeedsLoading() )
	{
		//CSha redBuff tmp(10);
		obytestream tmpStream;//( std::ios_base::in | std::ios_base::out );
		GetBytes( (ostream&)tmpStream, true, true );
	}

	return m_data->m_stmBuff.GetLength() > 0;//( m_data != nullptr && m_data.Length>0 );
}*/

void pdfBlob::EnsureLoaded( bool bDecrypt )
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 

	//pdfDocument doc( m_data->m_doc.lock() );
	CDocData& docData( *(m_data->m_doc.lock()) );

	// do we need to load the data from the stream?
	if (NeedsLoading())
	{
		pdfAtom atmLen = Get("Length");
		if (atmLen.IsInteger() && atmLen.Int() > 0)
		{
			try
			{
				int64 iCursor = docData.m_Parser->GetCursor();
				docData.m_stm.seekg( m_data->m_FilePos );

				m_data->m_stmBuff.Resize( atmLen.Int() );//= new byte[atmLen.Int()];
				//memset(m_data->m_stmBuff,0,atmLen.Int());
				m_data->m_nStmBytes = atmLen.Int();

				docData.m_stm.read( (char*)m_data->m_stmBuff.get(), m_data->m_nStmBytes);
				docData.m_Parser->SetCursor( iCursor );

			}
			catch(...)
			{
				// TODO do something!
			}
		}
		// TODO in Constants.h, make MinValueInt64 a #define
		m_data->m_FilePos = MinValueInt64;
	}


	// Handle Encryption - in this case decryption, actually
	//if ( bDecrypt && m_data!=nullptr && doc.IsEncrypted() && GetEncrypted() )
	//	SetEncrypted( !doc.GetSecurity()->DecryptObject( pdfAtom(*this), m_data->m_stmBuff ) );
	if ( bDecrypt && m_data!=nullptr && docData.m_bEncrypted && GetEncrypted() )
		SetEncrypted( !docData.m_pSecurity->DecryptObject( pdfAtom(*this), m_data->m_stmBuff ) );
}

void pdfBlob::GetBytes( std::ostream& stmOut, bool bLeaveEncoded, bool bDecrypt ) 
{
	if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
		throw pdfDocUndefinedException(); 

	//pdfDocument doc( m_data->m_doc.lock() );
	//CDocData& docData( *(m_data->m_doc.lock()) );

	EnsureLoaded(bDecrypt);
	//
	if (m_data->m_stmBuff.GetLength() > 0 && !GetEncrypted() && (!bLeaveEncoded && Has("Filter")))
	{
		Filter::pdfFilters filters(*this);

		filters.Transform(m_data->m_stmBuff, stmOut, true);
	}
	else
	{
		stmOut.write( (const char*)(m_data->m_stmBuff.get()), m_data->m_stmBuff.GetLength() );
	}
}
/*
void pdfBlob::SetData(CShare dBuff& bNewData, size_t nCount, Filter::pdfFilters filters)
{
	m_data = bNewData;
	m_data->m_FilePos = MinValueInt64;

	if (bNewData != nullptr && filters != nullptr && filters.Count>0 )
	{
		CSharedB uff data = filters.Encode(bNewData);
		if (data != nullptr && data.Length > 0 && data.Length < m_data.Length)
		{
			m_data = data;// filters.Encode( bNewData );

			if (m_data != nullptr)
				this["Filter"] = filters.CreateObj(m_data->m_doc);
		}
	}

	if ( filters == nullptr)
		this.m_dict.Remove( "Filter" );
	if (m_data == nullptr)
	{
		this.SetNumber("Length", 0);
		this.m_dict.Remove( "Filter" );
	}
	else
		this.SetNumber("Length", m_data.Length);
}

void pdfBlob::SetData(CS haredBuff bNewData, string sFilter )
{
	Filter::pdfFilters filters = new Filter::pdfFilters();
	filters.Add(sFilter);
	SetData(bNewData, filters);
}
public void SetData(byte[] bNewData )
{
	SetData(bNewData, nullptr as Filter::pdfFilters);
}

void pdfBlob::AppendData( byte[] bNewData )
{
	Filter::pdfFilters filters = nullptr;
	if ( Has("Filter") ) 
		filters = Filter::pdfFilters.GetFilters(this);
	if ( HasData )
		AppendData( bNewData, filters, false );
	else
		SetData( bNewData );
}
void pdfBlob::AppendData( byte[] bNewData, Filter::pdfFilters filters )
{
	AppendData( bNewData, filters, true );
};*/
/// <summary>
/// Remove any encoding from the stream
/// </summary>
/*void pdfBlob::Decode()
{
	CShare dBuff dat;
	GetBytes(dat,true);
	if (dat.get() != nullptr && Has("Filter"))
	{
		Filter::pdfFilters filters = Filter::pdfFilters.GetFilters(this);
	
		Remove("Filter");

		SetData(filters.Decode(dat), filters);
		filters = nullptr;
	}
}

void pdfBlob::AppendData( CSh aredBuff& bNewData, Filter::pdfFilters filters, bool bReplaceFilter )
{
	if ( bNewData != nullptr && bNewData.GetLength() > 0 )
	{
		byte[] bCurr = GetBytes( false );
		if ( bCurr != nullptr && bCurr.Length > 0 )
		{
			byte[] bResult = new byte[ bCurr.Length + bNewData.GetLength() ];
			Array.Copy( bCurr, 0, bResult, 0, bCurr.Length );
			Array.Copy( bNewData, 0, bResult, bCurr.Length, bNewData.GetLength() );

			m_data = bResult;

			if ( bReplaceFilter )
			{
				SetData( bResult, filters );
				return;
			}
			else if ( filters != nullptr && filters.Count > 0 )
			{
				byte[] data = filters.Encode( bResult );
				if ( data != nullptr && data.Length > 0 && data.Length < m_data.Length )
					m_data = data;
			}

			if ( m_data == nullptr )
			{
				this.SetNumber( "Length", 0 );
				this.m_dict.Remove( "Filter" );
			}
			else
				this.SetNumber( "Length", m_data.Length );

		}
		else if( bReplaceFilter )
			SetData( bNewData, filters );
		else
			SetData( bNewData );
	}
};
*/

};// namespace Exception
