#include "stdafx.h"
#include <stdlib.h>
#include "Model.h"
#include "Document.h"
#include "StdSecurity.h"
#include "pdfReader.h"
#include "Writer.h"
#ifdef _MSC_VER
#include <direct.h>
#endif
#ifndef _WINDOWS
#include <pwd.h>
#include <unistd.h>
#endif

using namespace Pdf::Security;

namespace Pdf
{

pdfDocument::pdfDocument(void)
{
	// we should always have a m_data, like pdfAtom does
	m_data.reset( new CDocData() );
}

pdfDocument::pdfDocument(const pdfDocument& src)
{
	operator=(src);
}

pdfDocument::pdfDocument(const CDocData::Ptr& src)
{
	operator=(src);
}

pdfDocument::~pdfDocument(void)
{
}

pdfDocument& pdfDocument::operator=(const pdfDocument& src)
{
	if( m_data.get() != src.m_data.get() )
	{
		m_data = src.m_data ;
		if ( m_data == nullptr )
			m_data.reset( new CDocData() );
	}
	return *this;
}

pdfDocument& pdfDocument::operator=(const CDocData::Ptr& src)
{
	if( m_data.get() != src.get() )
	{
		m_data = src ;
		if ( m_data == nullptr )
			m_data.reset( new CDocData() );
	}
	return *this;
}

pdfDocument& pdfDocument::operator=(const std::nullptr_t i)// specifically for disconnecting from the internal doc data - doesn't force a close
{
	m_data.reset( new CDocData() );
	return *this;
}

/*static*/ pdfDocument pdfDocument::NewDoc()// TODO add a version with a ostream*
{
	pdfDocument doc;
	doc.CreateNew();
	return doc;
}

pdfSecurityHandler* pdfDocument::GetSecurity()
{
	if ( m_data == nullptr )// right now, since we always make sure these exists, this shouldn't happen, but might change my mind later...
		throw pdfDocClosedException();
	if ( m_data->m_pSecurity == nullptr )
		m_data->m_pSecurity = new pdfStdSecurity( *this );
	return m_data->m_pSecurity;
}

void pdfDocument::SetSecurity(pdfSecurityHandler* pNew)
{
	if ( m_data == nullptr )
		throw pdfDocClosedException();
	if ( m_data->m_pSecurity != nullptr && m_data->m_bEncrypted && !m_data->m_pSecurity->CanChangePerms() )
		throw pdfUnauthorizedEx();
	//if( pNew != nullptr && !(pNew is pdfStdSecurity)) 
	//	throw pdfUnhandledSecurityEx();

	//if ( m_data->m_pNewSecurity != nullptr )
	//	delete m_data->m_pNewSecurity;
	m_data->m_pNewSecurity = pNew;

	m_data->m_bDirty = true;
}

void pdfDocument::CreateNew()//const char* szNewFile, bool bTemp)
{
	Close();
	try
	{
		// m_data should have been built in the constructor
		CDocData& doc( *(m_data.get()) );

		doc.m_bNew = true;

		/*doc.m_bTemp = bTemp;

		char szFile[260] = "";
		if( szNewFile == nullptr || strlen(szNewFile) == 0 )
		{
			doc.m_bTemp = true;

			char szCWD[260] = "";
	#ifdef _MSC_VER
			size_t nCount = 260;
			char* p = nullptr;
			_dupenv_s( &p, &nCount, "TEMP" ); 
			if( !p )
				strcpy_s( szFile, sizeof(szFile), "C:\\Temp\\" );
			else
			{
				strcpy_s( szFile, sizeof(szFile), p );
				free(p);
			}
			// change the working dir to the temp one (docs say tmpnam_s takes cwd into account when creating the name)
			_getcwd( szCWD, 260 );
			_chdir( szFile );
			::tmpnam_s( &szFile[strlen(szFile)], 260 - (strlen(szFile)) );
			_chdir( szCWD );// now change it back
			//::GetTempPathA()
			//::GetTempFileNameA( 
			strcat_s( szFile, 260, ".pdf" );
	#else
			//strcpy( szFile, getenv( "TEMP" )); 
			getcwd( szFile, 260 );
			//chdir( szFile );
			tmpnam( &szFile[strlen(szFile)] );
			//chdir( szCWD );// now change it back
			strcat( szFile, ".pdf" );
	#endif
		}
		else
		{
	#ifdef _MSC_VER
			strcpy_s( szFile, sizeof(szFile), szNewFile );
	#else
			strcpy( szFile, szNewFile );
	#endif
		}

		doc.m_stm.open( szFile, std::ios::out | std::ios::trunc | std::ios::binary );

		if (!doc.m_stm.bad() && doc.m_stm.is_open())
		{
			doc.m_filename = szFile;*/

			// Now we can/need to create the helper objects

			// our parser
			doc.m_Parser = new pdfReader( m_data, 0 );

			// and now our main dictionary - the trailer.  This is never pointed to by the xref table
			doc.m_Trailer = CreateDict( false );
			doc.m_Trailer.SetNumber( "Size", (int)1 );


			// Root - our Catalog object
			pdfDictionary dictRoot = CreateDict(true);
			dictRoot.SetName( "Type", "Catalog" );

			doc.m_Trailer.Set( "Root", dictRoot );

			// Info Block
			FillInfo();

			// ID array
			GenerateIDs();
		/*}
		else
		{
			if( doc.m_stm.is_open() )
				doc.m_stm.close();
			::remove( szFile );
		}*/
	}
	catch( const std::exception& ex )
	{
		Close();
		std::clog << "Exception while creating document: " << ex.what() << std::endl;
		throw;
	}
	catch( ... )
	{
		Close();
		std::clog << "Unknown exception while creating document." << std::endl;
		throw;
	}
}

// TODO Need to implement wide-char versions
void pdfDocument::Open( const wchar_t* szFile, const char* szPwd  )
{
	Close();
}

void pdfDocument::Open( const char* szFile, const char* szPwd )
{
	Close();

	try
	{
		// m_data should have been built in the constructor
		CDocData& doc( *(m_data.get()) );

		doc.m_stm.open( szFile, std::ios::in | std::ios::binary );

		if (!doc.m_stm.bad() && doc.m_stm.is_open())
		{
			doc.m_filename = szFile;
			doc.m_bTemp = false;

			//  return pdfDocument::Oepn ( doc.m_stm )// actually make an instance of a stream that the doc data can own 
			if( !ValidPDF() )
				throw pdfException("Not a valid PDF.");

			// Create all the help objects we need

			if( !Parse() ) 
			{
				throw pdfException("Unable to parse.");
				//Close();
				//return;
			}

			if ( !doc.m_Trailer.IsNull())
			{
				PasswordType bDecrypted =  PasswordType::None;

				// Handle encryption
				if ( doc.m_Trailer.Has( "Encrypt" ) )
				{
					pdfDictionary enc(doc.m_Trailer[ "Encrypt" ] );
					doc.m_bEncrypted = ( enc.Has( "U" ) && enc.Has( "O" ) && enc.Has( "R" ) && enc.Has( "P" ) && enc.Has( "Filter" ) ); // otherwise throw an except!
				}

				if ( doc.m_bEncrypted )
				{
					// the caller has some choices as far as authorizing:
					// They can simply provide a password string or
					// pass null and have already connected to the GetPassword event
					// and handle it there  by calling Security.Validate

					try
					{
						bDecrypted = GetSecurity()->Validate( szPwd );// even if this is null or blank, we mau only have an owner pwd

						// oh, well, try one of the delegates, if provided.
						/*if ( bDecrypted == PasswordType::Invalid )
						{
							// Can we ask for a password... like three times
							if ( fnGetPwd != null )
							{
								int iTries = 0;
								string sPwd = null;
								do
								{
									sPwd = fnGetPwd( this, sFile, iTries+1 );
									if ( sPwd == null )
										break;
									bDecrypted = GetSecurity()->Validate( sPwd );
								} while ( bDecrypted == PasswordType::Invalid && iTries++<3 );
							}

							// ok, ask them to decrypt.  Which they could since Security.Validate is public.
							if ( bDecrypted == PasswordType::Invalid && fnAuth != null )
								bDecrypted = fnAuth( this, sFile );
						}*/
					}
					catch ( const std::exception& e )
					{
						bDecrypted = PasswordType::Invalid;
#ifdef _DEBUG
						std::clog << "Error while opening '" << szFile << "': " << e.what();
#endif
						//ex = e;
					}

					if ( /*ex != null ||*/ bDecrypted == PasswordType::Invalid)
					{
						//Close();
						//if ( ex != null )
						//	throw new pdfNeedUserPwdEx( sFile, ex );
						throw pdfNeedUserPwdEx( szFile );
					}
				}

				if ( bDecrypted != PasswordType::Invalid )
				{
// TODO need an open eventn delegate
					//if ( this.Opened != null )
					//	this.Opened( this );

					//string s = ( doc.m_Trailer[ "Info" ] as pdfDictionary )[ "Producer" ].Str;
				//	return true;
				}
			}


			// trigger a page load
			//int iPgs = m_Pages.GetCount();
		}
		else
		{
			if( doc.m_stm.is_open() )
				doc.m_stm.close();
		}
	}
	catch( const std::exception& ex )
	{
		std::clog << "Exception while opening file: " << ex.what() << std::endl;
		Close();
		throw;
	}
	catch( ... )
	{
		std::clog << "Unhandled exception while opening file." << std::endl;
		Close();
		throw;
	}
}

bool pdfDocument::ValidPDF()
{
	if( m_data->m_stm.is_open() )
	{
		m_data->m_stm.seekg(0, std::ios_base::beg );
		const int iBuffSize = //(int)m_file.Length;
						1024;// (int)m_file.Length / 4;// (1 << 20) * 50;
		byte buff[iBuffSize] = {0};

		m_data->m_stm.read( (char*)buff, iBuffSize);
		if (m_data->m_stm.tellg() > 0)
		{
			int iRead = FindInBuff("%PDF", buff, iBuffSize, 0);
#if DEBUG
			//Debug.Print("%PDF at " + iRead.ToString());
#endif

			if( iRead == -1 )
				return false;

			float fMajor = (float)(buff[iRead + 5] - (byte)'0');
			float fMinor = (float)(buff[iRead + 7] - (byte)'0') / 10.0F;

			m_data->m_fVersion = fMajor + fMinor;

			m_data->m_sHeaderComments.clear();

			//iRead = FindInBuff("%", buff,7 );
			//if (iRead >= 0)
			//{
			//	//m_file.R
			//}

			return true;
		}
	}
	return false;
}

bool pdfDocument::Parse()
{
	bool bSuccess = false;
	//size_t x = 1 << 30;

	if( m_data->m_stm.is_open() )
	{
		m_data->m_stm.seekg(0, std::ios_base::end );
		size_t iFileLen = m_data->m_stm.tellg();
		//m_data->m_stm.seekg(0, std::ios_base::beg );

		if( iFileLen > 0 )
		{
			long iPos = iFileLen - 1024;
			if (iPos < 0)
				iPos = 0;

			size_t lBytes = iFileLen - iPos;

			std::unique_ptr<unsigned char[]> pBuff( new unsigned char[lBytes] );
			unsigned char* p = pBuff.get();
			memset( p,0,lBytes );//byte[] buff = new byte[lBytes];

			m_data->m_stm.seekg( iPos, std::ios_base::beg );
			//_ASSERT( iPos == m_data->m_stm.tellg() );

			m_data->m_stm.read( (char*)p, lBytes);			//bool b = m_data->m_stm.bad();
			lBytes = m_data->m_stm.gcount();

			int iRead = -1;
			if ( lBytes > 0 )
				iRead = FindInBuff("startxref", pBuff.get(), lBytes, 0 );//(int)iFileLen - 64);

			if (iRead >= 0)
			{
				// Now we can/need to create the helper objects
				m_data->m_Parser = new pdfReader( m_data, (int)iPos + iRead + 9 );

				//m_data->m_Parser->Reset( &m_data->m_stm, (int)iPos + iRead + 9 );

				m_data->m_Parser->Parse(  );

				// finally!
				ReadXRef();

				bSuccess = true;
			}
		}
	}
	return bSuccess;
}

void pdfDocument::ReadXRef()
{
	pdfAtom atm = m_data->m_Parser->ReadObj();

	if ( atm.GetType() == Keyword && atm.Str() == "xref" )
	{
		// Ok, we should get just two numbers at first.
		pdfAtom atmStartID = m_data->m_Parser->ReadObj();
		pdfAtom atmEntries = m_data->m_Parser->ReadObj();
		if ( atmStartID.IsInteger() && atmEntries.IsInteger() )
		{
			pdfAtom atmNull;
			pdfAtom atmOffset;// = m_lex.ReadObj();
			pdfAtom atmGenID;// = m_lex.ReadObj();
			pdfAtom atmKey;

			pdfObjId id;// = new pdfObjId();
			//int objID = 0;
			//ushort objGen = 0;
			int idStart = atmStartID.Int();
			id.SetId( idStart );
			int iCount = atmEntries.Int();

			do
			{
				for( int i = 0; i<iCount; i++ )//< while ( iCount-- > 0 )
				{
					atmOffset = m_data->m_Parser->ReadObj();
					atmGenID = m_data->m_Parser->ReadObj();
					atmKey = m_data->m_Parser->ReadObj();
					if ( !atmOffset.IsInteger() || !atmGenID.IsInteger() || atmKey.GetType() != Keyword )
						break;
					if ( atmKey.Str() == "n" )
					{
						id.SetId(  i + idStart );
						id.SetGenNum( atmGenID.Int() );
						id.SetFree( false );
						pdfReferences(m_data).Add( id, atmNull, atmOffset.Int() );
					}
					else if (atmKey.Str() == "f")
					{
						//int iSave
						id.SetId( atmOffset.Int() );
						id.SetGenNum( atmGenID.Int() );
						id.SetFree( true );
						//m_doc.m_XRef.Add( id, nullptr, atmOffset.Int() );

						if( id.GetGenNum() == 65535 )
							id.SetGenNum( 0 ); // actually, start a new linkedlist
						else
						{
							pdfReferences xref(m_data);
							xref.Add( id, atmNull, atmOffset.Int() );

							xref.SetNextFreeID( id );
						}
					}
					//id.ID++;
				}

				atm = m_data->m_Parser->ReadObj();
				if ( !atm.IsInteger() )
				{
					if ( atm.GetType() == Keyword && atm.Str() == "trailer" )
					{
						ReadTrailer();
					}
					break;
				}
				else
				{
					//
					atmStartID = atm;
					id.SetId( atmStartID.Int() );
					//
					atm = m_data->m_Parser->ReadObj();
					if ( !atm.IsInteger() )
						break;
					atmEntries = atm;
					iCount = atmEntries.Int();
					//atm = nullptr;
				}
			} while ( iCount > 0 );
		}
		//atm = nullptr;
	}
}

void pdfDocument::ReadTrailer()
{
	pdfAtom atm = m_data->m_Parser->ReadObj();
	if ( atm.GetType() == Dictionary )
	{
		pdfDictionary dictTrailer(atm);
		if( m_data->m_Trailer.IsNull() )
			m_data->m_Trailer = dictTrailer;

		if( dictTrailer.Has( "Prev" ) )
		{
			m_data->m_Parser->SetCursor( dictTrailer["Prev"].Int() );
			ReadXRef();
		}

#ifdef _DEBUG
		//DumpDict(Keys.trailer, m_data->m_Trailer);
#endif
	}
}

void pdfDocument::Close()
{
	if( IsOpen() )
	{
		try
		{
			// this will complete release everything and close the stream.
			m_data->Close();
		}
		catch( ... )
		{
		}

		// technically we could now re-use the current CDocData but we reset with a new one just to be safe.
		m_data.reset( new CDocData() );
	}
}
#include <time.h>
void pdfDocument::FillInfo()
{
	char szDate[64]="";

    struct tm *newtime;
#ifdef _MSC_VER
    int64 ltime;
	struct tm tmNew = {NULL};
	newtime = &tmNew;


    _time64( &ltime );
  
    // Obtain coordinated universal time:
    _gmtime64_s( newtime, &ltime ); 
//    newtime = _gmtime64( &ltime ); // C4996
	sprintf_s( szDate, 64, "D:%04d%02d%02d%02d%02d%02dZ", 
		newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec
		);
#else
	long int ltime;

	time( &ltime );

	// Obtain coordinated universal time:
	newtime = gmtime( &ltime ); // C4996
	sprintf( szDate, "D:%04d%02d%02d%02d%02d%02dZ", 
		newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec
		);
#endif


	pdfDictionary dict;//( m_data->m_Trailer["Info"] );
	if (!m_data->m_Trailer.Has("Info") ) //brand new - do it all, otherwise just mod date.
	{
		dict = CreateDict(true);
		m_data->m_Trailer.Set( "Info", dict );

		char szUser[64]="";
#ifdef _WINDOWS
		DWORD dw = 64;
		::GetUserNameA( szUser, &dw );
#else
		register struct passwd *pw;
		register uid_t uid;
		int c;

		uid = geteuid ();
		pw = getpwuid (uid);
		if (pw)
			strcpy( szUser, pw->pw_name);
#endif
		//dict["CreationDate"]

		//dict["CreationDate"] = Factory.CreateString(pdfDate.FormatDate(), pdfStringEncoding.ASCII, false);
		//dict["Title"] = Factory.CreateString(FileName, pdfStringEncoding.ASCII, false);
		//dict["Author"] = CreateString( Environment.UserName, pdfStringEncoding::ASCII, false );
		//dict["Creator"] = Factory.CreateString("PDFMgr", pdfStringEncoding.ASCII, false);
		//dict["Producer"] = CreateString("Glen Underwood", pdfStringEncoding::ASCII, false);

		// Or..... which looks better; is easy to use?
		dict.SetString("CreationDate", szDate, ASCII );
		dict.SetString("Title", m_data->m_filename.c_str(), ASCII );
		dict.SetString("Author", szUser , ASCII);
		dict.SetString("Creator", "PDF++", ASCII );
		dict.SetString("Producer", "Glen Underwood", ASCII );
	}
	else
		 dict = m_data->m_Trailer["Info"];

	//
	dict.SetString("ModDate", szDate );
	//dict["ModDate"] = CreateString(pdfDate.FormatDate(), pdfStringEncoding::ASCII, false);
}

#ifdef _MSC_VER
#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib" )
#endif

bool pdfDocument::GenerateIDs()
{
	if ( m_data == nullptr )
		throw pdfDocClosedException();

#ifdef _MSC_VER
	//RPC_CSTR* ppStr = nullptr;
	//UuidToStringA( &idNew, ppStr );

	if ( !m_data->m_Trailer.IsNull() && ( !m_data->m_Trailer.Has( "ID" ) || !m_data->m_Trailer[ "ID" ].IsArray() || pdfArray(m_data->m_Trailer[ "ID" ]).GetCount()<2) )
	{
		//Guid guid1 = Guid.NewGuid(), guid2 = Guid.NewGuid();
		UUID idNew1, idNew2;
		UuidCreate( &idNew1 );
		UuidCreate( &idNew2 );

		pdfArray arrID = CreateArray( 2, false );
		arrID.Set( 0, CreateHexString( (const byte*)&idNew1, sizeof(UUID), false ) );
		arrID.Set( 1, CreateHexString( (const byte*)&idNew2, sizeof(UUID), false ) );

		m_data->m_Trailer.Set("ID", arrID );
	}
	return !m_data->m_Trailer.IsNull() && m_data->m_Trailer.Has( "ID" ) && m_data->m_Trailer[ "ID" ].IsArray();
#else
	throw std::exception();
#endif
}

std::vector<byte> pdfDocument::GetID1()
{
	if ( m_data == nullptr )
		throw pdfDocClosedException();
	std::vector<byte> buff;
	if( m_data->m_Trailer.Has("ID") )
	{
		pdfArray aID( m_data->m_Trailer[ "ID" ] );
		if ( !aID.IsNull() && aID.GetCount() > 1 )
		{
			pdfString str( aID[ 0 ] );
			if ( !str.IsNull() )
			{
				buff.resize( str.GetData()->m_hexData.GetLength() );
				memcpy( &buff[0], str.GetData()->m_hexData.get(), str.GetData()->m_hexData.GetLength() );
			}
		}
	}
	return buff;
}

std::vector<byte> pdfDocument::GetID2()
{
	if ( m_data == nullptr )
		throw pdfDocClosedException();
	std::vector<byte> buff;
	if( m_data->m_Trailer.Has("ID") )
	{
		pdfArray aID( m_data->m_Trailer[ "ID" ] );
		if ( aID.IsNull() && aID.GetCount() > 1 )
		{
			pdfString str( aID[ 1 ] );
			if ( !str.IsNull() )
			{
				buff.resize( str.GetData()->m_hexData.GetLength() );
				memcpy( &buff[0], str.GetData()->m_hexData.get(), str.GetData()->m_hexData.GetLength() );
			}
		}
	}
	return buff;
}

// IDocument
//void pdfDocument::AtomDataCreated(CAtomData* pNew)
//{
//	m_data->AtomDataCreated( pNew );
//}
//void pdfDocument::RemoveAtomData(CAtomData* pDel)
//{
//	m_data->RemoveAtomData( pDel );
//}

pdfInt pdfDocument::CreateInteger(int i, bool bIndirect)
{
	pdfInt atmNew( *this, i);
	if( bIndirect )
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfInt64 pdfDocument::CreateInteger64(int64 i, bool bIndirect)
{
	pdfInt64 atmNew( *this, i);
	if( bIndirect )
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfReal pdfDocument::CreateReal(float d, int prec, bool bIndirect)
{
	pdfReal atmNew( *this, d, prec);
	if( bIndirect )
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfReal pdfDocument::CreateReal(double d, int prec, bool bIndirect)
{
	pdfReal atmNew( *this, d, prec);
	if( bIndirect )
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfBool pdfDocument::CreateBoolean(bool Val, bool bIndirect)
{
	pdfBool atmNew( *this, Val);
	if (bIndirect)
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfString pdfDocument::CreateString(const char* Val, StringEncoding encoding, bool bIndirect)
{
	pdfString atmNew( *this, Val, encoding);
	if (bIndirect)
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfString pdfDocument::CreateHexString( const byte* Val, size_t iSize/*, bool bTakeOwnership*/, bool bIndirect)
{
	pdfString atmNew( *this, Val, iSize/*, bTakeOwnership*/);
	if (bIndirect)
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfName pdfDocument::CreateName(const char* Val, bool bIndirect)
{
	pdfName atmNew( *this, Val);
	if (bIndirect)
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfDictionary pdfDocument::CreateDict(bool bIndirect)
{
	pdfDictionary atmNew( *this);
	if( bIndirect )
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfBlob pdfDocument::CreateStream()
{
	pdfBlob atmNew( *this);
	pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfArray pdfDocument::CreateArray(const size_t iStartingCount, bool bIndirect)
{
	pdfArray atmNew( *this,iStartingCount);
	if( bIndirect )
		pdfReferences(m_data).Add(atmNew, MinValueInt64);
	return atmNew;
}

int pdfDocument::FindInBuff(const char* str, unsigned char* pBuff, size_t iBuffSize, int iStart)
{
	int i = -1, x = iStart;
	size_t iStrLen = strlen(str);

	//  string s = new string(buff, 0, 4, Encoding.ASCII);
	// string s;// = (string)buff;
	for (x = iStart; x < iBuffSize; x++ )
	{
		if (str[0] == (char)pBuff[x] && iBuffSize-x >= iStrLen )
		{
			bool b = true;
			//string s = str.Substring(0, 1);
			for (i = 1; i < iStrLen; i++)
			{
				if (str[i] != (char)pBuff[x + i])
				{
					b = false;
					break;
				}
			}
			if (b)
			{
				return x;
			}
		}
	}
	return -1; // str.IndexOf(s);
}

};//Pdf
