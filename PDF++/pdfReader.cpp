#include "stdafx.h"
#include <stdlib.h>
#include <ctype.h>
#include "pdfReader.h"
#include "Model.h"
#include "Document.h"

#define IsDigit( c ) (c >= '0' && c <= '9')

namespace Pdf {
//namespace Utils {

pdfReader::pdfReader() 
{
	m_stm = nullptr;
	_Previous = (char)0xFFFF;
	_Current = (char)0xFFFF;
	_Next = (char)0xFFFF;
	_iCursor=0;
	m_strDefStrEncoding = StringEncoding::ASCII;
	m_bAssumeDocIsEncrypted = false;
	throw pdfNotImplException();
}


/*
pdfReader::pdfReader(std::shared_ptr<std::istream> stm, std::shared_ptr<pdfFactory> fact)
{
	throw pdfNotImplException();
}*/

///////////////////////////////////////////////////////

pdfReader::pdfReader( const CDocData::Ptr& doc, int iNewCursor )
{
	m_doc = doc;

	if( m_doc.expired() || m_doc.lock() == nullptr)
		throw pdfException("Reader requires a valid document object to function.");

	m_strDefStrEncoding = StringEncoding::ASCII;
	
	m_bAssumeDocIsEncrypted = true;

	Reset( &(m_doc.lock()->m_stm), iNewCursor );
}

pdfReader::pdfReader( const CDocData::Ptr& doc, std::istream* pStm, int iNewCursor )
{
	m_doc = doc;

	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");

	m_strDefStrEncoding = StringEncoding::ASCII;
	
	m_bAssumeDocIsEncrypted = true;

	Reset( pStm, iNewCursor );
}

void pdfReader::SetCursor( int iCursor )
{
	_iCursor = iCursor;
	_Previous = (char)0xFFFF;
	_Current = (char)0xFFFF;
	_Next = (char)0xFFFF;
	if ( m_stm!=nullptr )
	{
		if( /*m_stm == nullptr || */m_doc.expired() )
			throw pdfException("Reader requires a valid document object to function.");
		m_stm->seekg( _iCursor );
		m_stm->read( &_Current, 1 ) ;//= (char)m_Stm.ReadByte();
		m_stm->read( &_Next, 1 ) ;//= (char)m_Stm.ReadByte();
	}
}

pdfReader::~pdfReader(void)
{
}

void pdfReader::Parse()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	//if (m_doc.lock()->IsOpen() )
	{
		pdfAtom atm = ReadNum();
		if ( atm.GetType() == Integer )
		{
			int i =0;
			atm.GetValue(i);
			SetCursor( i );

			//ReadXRef();
		}
	}
}

void pdfReader::Reset(std::istream* stm, int iNewCursor)
{
//	if( m_doc.expired() )
//		throw pdfException("Reader requires a valid document object to function.");

	m_stm = stm;

	SetCursor(iNewCursor);
}

bool pdfReader::IsWhiteSpace(char c) const throw()
{
	//if (c == 0)
	//	return true;
	switch (c)
	{
		case (char)0:
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\f':
			return true;
	}
	return false;
}

bool pdfReader::IsDelimiter(char c) const throw()
{
	switch (c)
	{
		case '(':
		case ')':
		case '/':
		case ' ':
		case '>':
		case '<':
		case '[':
		case ']':
			return true;
	}
	return false;
}

char pdfReader::GotoNextChar()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	if (m_doc.lock()->IsOpen() )
	{
		_Previous = _Current;
		_Current = _Next;

		// m_stm->read may not change this (especially if we've hit eof)
		// so assume eof 
		_Next = (char)0xFFFF;
		if( !m_stm->eof() )
		{
			m_stm->read( &_Next, 1 ) ;//= (char)m_Stm.ReadByte();
			_iCursor++;
		}
	}
	else
	{
		_Previous = (char)0xFFFF;
		_Current = (char)0xFFFF;
		_Next = (char)0xFFFF;
	}
	return _Current;
}

bool pdfReader::IsAtHexByte() const throw()
{
	if (IsDigit(_Current)
		|| _Current == 'A' || _Current == 'a'
		|| _Current == 'B' || _Current == 'b'
		|| _Current == 'C' || _Current == 'c'
		|| _Current == 'D' || _Current == 'd'
		|| _Current == 'E' || _Current == 'e'
		|| _Current == 'F' || _Current == 'f'
		)
	{
		if (IsDigit(_Next)
			|| _Next == 'A' || _Next == 'a'
			|| _Next == 'B' || _Next == 'b'
			|| _Next == 'C' || _Next == 'c'
			|| _Next == 'D' || _Next == 'd'
			|| _Next == 'E' || _Next == 'e'
			|| _Next == 'F' || _Next == 'f'
			)
			return true;
	}
	return false;
}

bool pdfReader::IsValidKeyword(const char* sVal)
{
	return (sVal == "xref" || sVal == "startxref" || sVal == "R" || sVal == "obj" || sVal == "endobj" ||
		sVal == "stream" || sVal == "endstream" || sVal == "trailer" || sVal == "f" || sVal == "n" ||
		sVal == "true" || sVal == "false" || sVal == "null");
}

void pdfReader::ReadEscapeChar()
{
	if (_Current == '\\')
	{
		GotoNextChar();
		switch (_Current)
		{
			case '\n':
			case '\r':
				while (_Current == '\n' || _Current == '\r')//strings that have been spread across lines.
					GotoNextChar();
				//if (Current == ')')
				//	Debug.Print("");
				ReadEscapeChar();
				break;
			case 'n':
				_Current = '\n';
				break;
			case 't':
				_Current = '\t';
				break;
			case 'r':
				_Current = '\r';
				break;
			case 'b':
				_Current = '\b';
				break;
			case 'f':
				_Current = '\f';
				break;
			/*case ')':
			case '(':
				return;*/

			// Octal number?
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				{
					//int iCursorSave = Cursor;
					char a = '0';
					char b = '0';
					char c = _Current;
					//GotoNextChar();
					if (_Next >= '0' && _Next <= '7')
					{
						GotoNextChar();
						b = c;
						c = _Current;
						if (_Next >= '0' && _Next <= '7')
						{
							GotoNextChar();
							a = b;
							b = c;
							c = _Current;
						}
					}

					int x = ((a - '0') * 64) + ((b - '0') * 8) + (c - '0');
					_Current = (char)x;
				}
				break;
			//default:
			//	throw pdfMgrException( "Unrecognized escape." );
		}
		//Debug.Assert(Utils.IsDelimiter(Current));

	}
}

void pdfReader::SkipToEOL()
{
	while (_Current != (char)0xFFFF && (_Current == '\n' || _Current == '\r'))
	{
		GotoNextChar();
	}
}

void pdfReader::SkipComment()
{
	while (_Current != (char)0xFFFF && _Current != '\n' && _Current != '\r')
	{
		GotoNextChar();
	}
	SkipToEOL();
}

void pdfReader::SkipWhiteSpace()
{
	while (_Current != (char)0xFFFF && IsWhiteSpace(_Current))
		GotoNextChar();
}

byte pdfReader::FromHexDigit(char cDigit)
{
	if (cDigit >= '0' && cDigit <= '9')
	{
		return (byte)(cDigit - '0');
	}
	else if (cDigit >= 'a' && cDigit <= 'f')
	{
		return (byte)((cDigit - 'a') + 10);
	}
	else if (cDigit >= 'A' && cDigit <= 'F')
	{
		return (byte)((cDigit - 'A') + 10);
	}
	return 0;
}

pdfAtom pdfReader::ReadNum()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	if (m_doc.lock()->IsOpen() )
	{
		pdfDocument doc(m_doc.lock());

		SkipWhiteSpace();

		bool bFloat = false;
		int iBytes = 0;
		std::string sVal ;

		while (_Current != (char)0xFFFF && (IsDigit(_Current) || _Current == '.' || _Current == '-') && !IsWhiteSpace(_Current) && !IsDelimiter(_Current))
		{
			iBytes++;
			bFloat = bFloat || _Current == '.';

			sVal += _Current;

			GotoNextChar();
		}

		if (iBytes > 0)
		{
#ifdef _DEBUG
			//Debug.Print("ReadNum = {0}", sVal);// + iRead.ToString());
#endif
			if (!bFloat)
			{
//__cplusplus;



				if (sVal.length() > 10)
				{
#ifdef __GNUC__
					/*auto delegate = [] (const char *instr)
						{
							long long retval;
							int i;

							retval = 0;
							for (; *instr; instr++)
								retval = 10*retval + (*instr - '0');
							return retval;
						};
					return doc.CreateInteger64( delegate( sVal.c_str() ), false);*/
					// this is equivalent to the above, but not as easy to read.  Defining a delegate and calling it all in one shot.
					return doc.CreateInteger64( [] (const char *instr)
														{
															long long retval = 0;
															for (; *instr; instr++)
																retval = 10*retval + (*instr - '0');
															return retval;
														}( sVal.c_str() ),
														false );
#else // _MSC_VER
					return doc.CreateInteger64( _atoi64(sVal.c_str()), false);
#endif
				}
				else
					return doc.CreateInteger( atoi(sVal.c_str()), false);
			}
			else
			{
				/*_CRT_DOUBLE d = { NULL};
				_atodbl( &d, (char*)sVal.c_str() );
				return doc.CreateReal( d.x, false);*/
				return doc.CreateReal( atof(sVal.c_str()), false);
			}
		}
	}
	return pdfAtom( );
}

pdfAtom pdfReader::ReadName()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	if (m_doc.lock()->IsOpen() )
	{
		pdfDocument doc(m_doc.lock());

		SkipWhiteSpace();

		bool bFloat = false;
		size_t iBytes = 0;
		byte hi = 0, lo = 0;
		obytestream stmBytes;

		while (_Current != (char)0xFFFF && !IsWhiteSpace(_Current))
		{
			// also check for # which indicates a char ID (or maybe it would just be digits?)
			if (_Current == '#')
			{
				if (_Next == '#')// ERROR - skip it
					GotoNextChar();

				GotoNextChar();

				while (IsAtHexByte())
				{
					hi = FromHexDigit( _Current );
					lo = FromHexDigit( (isalnum( _Next ) ? _Next : 0) );

					stmBytes.put( (byte)( ( hi << 4 ) | ( lo & (byte)0x0F ) ) );

					//
					GotoNextChar();
					GotoNextChar();
					if (_Current != '#')
						break;
					GotoNextChar();
				}

				/*byte* pBuff = nullptr;
				size_t nLen = 0;
				mem.ToArray(&pBuff, &nLen);
				iBytes += nLen;
				//std::unique_ptr<byte> pBuff( new byte[32000] )
				if( pBuff )
				{
					sVal += (char*)pBuff;
					delete [] pBuff;
				}*/
				continue;

			}
			else if (IsDelimiter(_Current))
				break;
			iBytes++;
			stmBytes.put( _Current );

			GotoNextChar();
		}

		if (stmBytes.GetLength() > 0)
		{
			pdfName newStr( doc, string( (char*)stmBytes.GetBuffer(), stmBytes.GetLength()).c_str() );
			//names are never encrypted
			/*if ( m_bAssumeDocIsEncrypted && doc.IsEncrypted() )
			{
				newStr.SetEncrypted( doc.IsEncrypted() );
				newStr.SetStringEncoding( StringEncoding::ByteAsChars );
			}*/

			return newStr;
		}
		return doc.CreateName("", false);
	}
	return pdfAtom();
}

pdfAtom pdfReader::ReadString()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	if (m_doc.lock()->IsOpen() )
	{
		pdfDocument doc(m_doc.lock());
		size_t iBytes = 0;

		StringEncoding strType = m_strDefStrEncoding;// StringEncoding::ASCII;

		obytestream stmStrBytes;

		if( _Current != (char)0xFFFF )
		{
			// For encryption - just read in the bytes raw
			if ( doc.IsEncrypted() )
			{
				while( _Current != (char)0xFFFF )
				{
					if ( _Current == '\\' )
					{
						ReadEscapeChar();
					}
					else if ( _Current == ')' )
					{
						GotoNextChar();
						break;
					}

					stmStrBytes.put( _Current );

					GotoNextChar();
				}
			}
			// ===== Unicode Little-Endian
			else if( _Current == (char)0xFF && _Next == (char)0xFE )
			{
				strType = StringEncoding::UTF16LE;
				GotoNextChar(); GotoNextChar();
				while( _Current != (char)0xFFFF && _Next != (char)0xFFFF )
				{
					char a, b;
					if (_Current == '\\')
					{
						ReadEscapeChar();
					}
					else if (_Current == ')')
					{
						GotoNextChar();
						break;
					}
					a = _Current;
					GotoNextChar();

					//
					if (_Current == '\\')
					{
						ReadEscapeChar();
					}
					else if (_Current == ')')
					{
						GotoNextChar();
						break;
					}
					b = _Current;
					GotoNextChar();

					//System.Char.ConvertFromUtf32(
					//((((unsigned short)*p) & 0xFF00)>>8) | ((((unsigned short)*p) & 0x00FF)<<8)
				//	string x = System.Char.ConvertFromUtf32((((byte)a) << (byte)8) | ((byte)b));
					//s.Append(x);

					iBytes++;

					//memStringBuff.WriteByte((byte)a);
					//memStringBuff.WriteByte((byte)b);
					stmStrBytes.put( a );
					stmStrBytes.put( b );

				}
			}
			// ===== Unicode Big-Endian
			else if( _Current == (char)0xFE && _Next == (char)0xFF )
			{
				// we're reading big-endian
				strType = StringEncoding::UTF16BE; 

				GotoNextChar(); GotoNextChar();
				while( _Current != (char)0xFFFF && _Next != (char)0xFFFF )
				{
					char a, b;
					if (_Current == '\\')
					{
						ReadEscapeChar();
					}
					else if (_Current == ')')
					{
						GotoNextChar();
						break;
					}
					a = _Current;
					GotoNextChar();

					//
					if (_Current == '\\')
					{
						ReadEscapeChar();
					}
					else if (_Current == ')')
					{
						GotoNextChar();
						break;
					}
					b = _Current;
					GotoNextChar();

					//string x = System.Char.ConvertFromUtf32((((byte)b) << 8) | ((byte)a));
					//s.Append(x);

					iBytes++;

					//memStringBuff.WriteByte((byte)a);
					//memStringBuff.WriteByte((byte)b);
					stmStrBytes.put( a );
					stmStrBytes.put( b );
				}
			}
			// =====
			else
			{
				while( _Current != (char)0xFFFF )
				{
					if (_Current == '\\')
					{
						ReadEscapeChar();
					}
					if( _Current == ')' && _Previous != '\\' )
					{
						GotoNextChar();
						break;
					}
					iBytes++;

					if (_Current > 127 && strType == StringEncoding::ASCII )
						strType = StringEncoding::PDFDocEncoding; 

					//memStringBuff.WriteByte((byte)_Current);
					stmStrBytes.put( _Current );

					GotoNextChar();
				}
			}
		}
		
		if (stmStrBytes.GetLength() > 0)
		{
			// TODO Need to get as a byte*.  Customize stringbuf

			pdfString newStr( doc, stmStrBytes.GetBuffer(), stmStrBytes.GetLength(), strType );

			if ( m_bAssumeDocIsEncrypted && doc.IsEncrypted() )
			{
				newStr.SetEncrypted( doc.IsEncrypted() );
				newStr.SetStringEncoding( StringEncoding::ByteAsChars );
			}

			return newStr;
		}
		return doc.CreateString("", StringEncoding::ASCII, false);
	}
	//return nullptr;// doc.CreateString("", StringEncoding::ASCII, false);
	return pdfAtom();
}

pdfAtom pdfReader::ReadHexString()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	if (m_doc.lock()->IsOpen() )
	{
		pdfDocument doc(m_doc.lock());
		SkipWhiteSpace();

		obytestream stmHexBytes;

		byte hi = 0, lo = 0;

		while ( _Current != (char)0xFFFF )
		{
			if ( _Current == '>' )
			{
				GotoNextChar();
				break;
			}

			hi = FromHexDigit( _Current );
			lo = FromHexDigit( (isalnum( _Next ) ? _Next : 0) );
/*#ifdef _DEBUG
			if ( !System.Char.IsLetterOrDigit( _Current ) || !System.Char.IsLetterOrDigit( Next ) )
				Debug.Print( string.Empty );
			if ( hi > 127 || lo > 127 )
				Debug.Print( string.Empty );
			sb.Append( _Current );
			sb.Append( Next );

			//string s = string.Format( "{0}{1}", _Current, Next );
			//int x = int.Parse( s, System.Globalization.NumberStyles.HexNumber );
			//Debug.Print( x.ToString( "X2" ) );
#endif*/
			//c = (byte)( ( a & (byte)0x0F ) << 4 );
			//c = (byte)( c | ( b & (byte)0x0F ) );

			//c = (byte)( ( hi << 4 ) | ( lo & (byte)0x0F ) );

			stmHexBytes.put( (byte)( ( hi << 4 ) | ( lo & (byte)0x0F ) ) );

			//
			GotoNextChar();
			if ( _Current == (char)0xFFFF )//file may have been truncated
				break;
			if ( _Current == '>' )// might be missing the lower nybble
			{
				GotoNextChar();
				break;
			}
			GotoNextChar();

			//	System.Char.Parse(sNum.ToString());
		}

		if (stmHexBytes.GetLength() > 0)
		{
			// TODO Need to get as a byte*.  Customize stringbuf

			pdfString newStr( doc, stmHexBytes.GetBuffer(), stmHexBytes.GetLength(), StringEncoding::Byte );

			if ( m_bAssumeDocIsEncrypted && doc.IsEncrypted() )
			{
				newStr.SetEncrypted( doc.IsEncrypted() );
				newStr.SetStringEncoding( StringEncoding::ByteAsChars );
			}

			return newStr;
		}
		return doc.CreateString("", StringEncoding::ASCII, false);
		/*

		byte* pBuff = nullptr;
		size_t nLen = 0;
		mem.ToArray(&pBuff, &nLen);
		if( pBuff )
		{
			pdfString strHex( doc.CreateHexString( pBuff, nLen, false ) );
			delete [] pBuff;
			return strHex;
		}*/
	}
	return pdfAtom();
}

pdfAtom pdfReader::ReadDict()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	pdfDocument doc(m_doc.lock());
	pdfDictionary dictNew( doc.CreateDict(false) );
	if (m_doc.lock()->IsOpen() )
	{
		SkipWhiteSpace();

		pdfAtom atmKey, atmVal, atmGen, atmRef;
		while (_Current != (char)0xFFFF && !(_Current == '>' && _Next == '>'))
		{
			atmKey = ReadObj();
			atmVal = ReadObj();
			// ASSERT/EXCEPT IF THISISN"T TRUE!
			if (atmKey.GetType() == Name && atmVal.GetType() != Null)
			{
				if (atmVal.GetType() == Integer)
				{
					int iCurs = _iCursor;
					atmGen = ReadObj();
					if (atmGen.GetType() == Integer)
					{
						atmRef = ReadObj();
						if (atmRef.GetType() == Keyword && atmRef.Str() == "R")
						{
							// object reference

							// SEARCH THE REF TABLE!
							//pdfAtom newVal = m_doc.FindReferencedObj(new pdfObjId(atmVal.Int, (ushort)atmGen.Int));

							pdfReference refer(doc, new pdfObjId(atmVal.Int(), (unsigned short)atmGen.Int()));
							//Debug.Assert(refer != null);
							atmVal = refer;
						}
						else
							SetCursor( iCurs );
					}
					else
						SetCursor( iCurs );
				}

				//
				dictNew.Set( atmKey.Str().c_str(), atmVal );
			}

			SkipWhiteSpace();
		}

		if (_Current == '>' && _Next == '>')
		{
			GotoNextChar();
			GotoNextChar();
		}
	}

	// this could be a stream object
	if (dictNew.GetCount() > 0)
	{
		int iCurs = _iCursor;
		pdfAtom atm1 = ReadObj();
		if (  atm1.GetType() == Keyword && atm1.Str() == "stream")
		{
			SkipToEOL();

			//atmVal._type = atomStream;//
			pdfBlob stmNew(dictNew, _iCursor );// doc.Factory.CreateStream();

/*			pdfBlob stmNew(m_doc);// doc.Factory.CreateStream();
			stmNew.m_dict = dictNew.m_dict;
			stmNew.m_lBytePos = _iCursor;*/
			//if( stmNew.Has("Filter"))
			//	stmNew.Encoded = true;

			// PUT THIS IN Stream.GetData !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//atm1 = stmNew["Length"];
			//if (atm1 != null && atm1.IsInteger() && atm1.Int > 0)
			//{
			//    SkipToEOL();
			//    m_Stm.Position = Cursor;
			//    stmNew.m_data = new byte[atm1.Int];
			//    m_Stm.Read( stmNew.m_data, 0, stmNew.m_data.Length );
			//}
			SetCursor(iCurs);

			// For enc
#pragma message("TODO")
			// TODO Encryptino
			//if ( m_bAssumeDocIsEncrypted && doc.IsEncrypted() )
			//	stmNew.Encrypted = doc.IsEncrypted();

			return stmNew;
		}
		SetCursor(iCurs);
		//atm2 = ReadObj();
		//Debug.Print(atm1.GetType().ToString());
	}

	return dictNew;
}

pdfAtom pdfReader::ReadArray()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	pdfAtom atmNew;
	if (m_doc.lock()->IsOpen() )
	{
		pdfDocument doc(m_doc.lock());
		atmNew = doc.CreateArray(0, false);

		SkipWhiteSpace();

		pdfAtom atmVal;
		while (_Current != (char)0xFFFF && _Current != ']')
		{
			atmVal = ReadObj();
			SkipWhiteSpace();
			if ( !atmVal.IsNull() )
			{
				//
				// object reference?
				if (atmVal.GetType() == Integer)
				{
					int iCurs = _iCursor;
					pdfAtom atmGen = ReadObj();
					if (atmGen.GetType() == Integer)
					{
						pdfAtom atmRef = ReadObj();
						if (atmRef.GetType() == Keyword && atmRef.Str() == "R")
						{
							// SEARCH THE REF TABLE!
							//atmVal = m_doc.FindReferencedObj(new pdfObjId(atmVal.Int, (ushort)atmGen.Int));
							pdfObjId id(atmVal.Int(), atmGen.Int());
							pdfReference refer(doc, &id );
							//Debug.Assert(refer != null);
							atmVal = refer;
						}
						else
							SetCursor( iCurs );
					}
					else
						SetCursor( iCurs );
				}

				// ASSERT/EXCEPT IF THISISN"T TRUE!
				if ( !atmVal.IsNull() )
				{
#pragma message("TODO")
					// For enc
					//if ( doc.IsEncrypted() )
					//	atmVal.Encrypted = doc.IsEncrypted();

					pdfArray(atmNew).Add( atmVal );
				}
			}
		}
		if (_Current == ']')
			GotoNextChar();
	}
	return atmNew;
}

pdfAtom pdfReader::ReadObj()
{
	if( /*m_stm == nullptr || */m_doc.expired() )
		throw pdfException("Reader requires a valid document object to function.");
	if (m_doc.lock()->IsOpen() )
	{
		pdfDocument doc(m_doc.lock());

		SkipWhiteSpace();

		while (_Current == '%')
		{
			SkipComment();
			SkipWhiteSpace();
		}

		// remember our pos in the stream in case
		// because if we cannot determine what type it is, we have to read all teh bytes through to figure out if it's a number or text.  If we discover
		// it's a number I reset the cursor to the anchor, and use ReadNum to properly pull it in.
		int iAnchor = _iCursor;

		// String?
		if (_Current == '(')
		{
			GotoNextChar();
			return ReadString();
		}

		// Name?
		if (_Current == '/')
		{
			GotoNextChar();
			return ReadName();
		}

		// Dictionary?
		if (_Current == '<' && _Next == '<')
		{
			GotoNextChar();
			GotoNextChar();
			return ReadDict();
		}

		// Hex String?  (make sure to test after testing for a dict.
		if (_Current == '<')
		{
			GotoNextChar();
			return ReadHexString();
		}

		// array?
		if (_Current == '[')
		{
			GotoNextChar();
			return ReadArray();
		}

		// not sure what we've got here so try to guess by reading everything until some whitespace
		// probably a number or some keyword, hopefully.

		std::string s;
		bool bDigits = false, bAlpha = false, bPunct = false;
		int iBytes = 0;
		while ( _Current != (char)0xFFFF && !IsWhiteSpace( _Current ) && !IsDelimiter( _Current ) )//&& (System.Char.IsDigit(Current) || Current == '.' || Current == '-'))
		{
			iBytes++;
			bDigits = bDigits || IsDigit( _Current );
			bPunct = bPunct || _Current == '.' || _Current == '-' || _Current == '+';
			bAlpha = bAlpha || ( !bPunct && !IsDigit( _Current ) );
			s += _Current ;
			GotoNextChar();
		}

		if (iBytes > 0)
		{
			//Debug.Print(s.ToString());

			if (bDigits && !bAlpha)
			{
				SetCursor( iAnchor );
				return ReadNum();
			}

			// ok, some sort of keyword
			std::string sBuff;
			std::transform(	s.begin(),s.end(),
							std::back_inserter(sBuff),
							[] (char a) { return toupper(a); });
#ifdef __GNUC__
#else
			/*char* szBuff = new char[s.size()+1];
			memset(szBuff,0,s.size()+1);
			strcpy_s( szBuff, s.size()+1, s.c_str() );
			_strupr_s( szBuff, s.size()+1 );
			sBuff = szBuff;
			delete [] szBuff;*/
#endif

			if (strcmp( sBuff.c_str(), "FALSE")==0)
			{
				return pdfBool( doc, false);
			}
			else if (strcmp( sBuff.c_str(), "TRUE")==0)
			{
				return pdfBool( doc, true );
			}
			else if (strcmp( sBuff.c_str(), "NULL")==0)
			{
				return pdfAtom( doc );
			}

#if _DEBUG
			//Debug.Assert(IsValidKeyword(sVal));
#endif
			// TODO TODO TODO TODO TODO TODO   encoding
#ifdef _WINDOWS
			pdfString atm( doc, s.c_str(), WinAnsiEncoding);
#else
			pdfString atm( doc, s.c_str(), ASCII);
#endif
			atm.m_data->m_hexData.Clear();
			atm.m_data->m_strName = s;
			atm.m_data->m_type = Keyword;
			return atm;
		}
	}
	// always return something (?) - makes testing a little easier.
	return pdfAtom();//null;
}

//void pdfReader::ReadXRef()
//{
//	pdfAtom atm = ReadObj();
//
//	if ( atm.GetType() == Keyword && atm.Str() == "xref" )
//	{
//		// Ok, we should get just two numbers at first.
//		pdfAtom atmStartID = ReadObj();
//		pdfAtom atmEntries = ReadObj();
//		if ( atmStartID.IsInteger() && atmEntries.IsInteger() )
//		{
//			pdfAtom atmNull;
//			pdfAtom atmOffset;// = m_lex.ReadObj();
//			pdfAtom atmGenID;// = m_lex.ReadObj();
//			pdfAtom atmKey;
//
//			pdfObjId id;// = new pdfObjId();
//			//int objID = 0;
//			//ushort objGen = 0;
//			int idStart = atmStartID.Int();
//			id.SetId( idStart );
//			int iCount = atmEntries.Int();
//
//			do
//			{
//				for( int i = 0; i<iCount; i++ )//< while ( iCount-- > 0 )
//				{
//					atmOffset = ReadObj();
//					atmGenID = ReadObj();
//					atmKey = ReadObj();
//					if ( !atmOffset.IsInteger() || !atmGenID.IsInteger() || atmKey.GetType() != Keyword )
//						break;
//					if ( atmKey.Str() == "n" )
//					{
//						id.SetId(  i + idStart );
//						id.SetGenNum( atmGenID.Int() );
//						id.SetFree( false );
//						doc.CrossReference().Add( id, atmNull, atmOffset.Int() );
//					}
//					else if (atmKey.Str() == "f")
//					{
//						//int iSave
//						id.SetId( atmOffset.Int() );
//						id.SetGenNum( atmGenID.Int() );
//						id.SetFree( true );
//						//m_doc.m_XRef.Add( id, nullptr, atmOffset.Int() );
//
//						if( id.GetGenNum() == 65535 )
//							id.SetGenNum( 0 ); // actually, start a new linkedlist
//						else
//						{
//							doc.CrossReference().Add( id, atmNull, atmOffset.Int() );
//
//							doc.CrossReference().SetNextFreeID( id );
//						}
//					}
//					//id.ID++;
//				}
//
//				atm = ReadObj();
//				if ( !atm.IsInteger() )
//				{
//					if ( atm.GetType() == Keyword && atm.Str() == "trailer" )
//					{
//						ReadTrailer();
//					}
//					break;
//				}
//				else
//				{
//					//
//					atmStartID = atm;
//					id.SetId( atmStartID.Int() );
//					//
//					atm = ReadObj();
//					if ( !atm.IsInteger() )
//						break;
//					atmEntries = atm;
//					iCount = atmEntries.Int();
//					//atm = nullptr;
//				}
//			} while ( iCount > 0 );
//		}
//		//atm = nullptr;
//	}
//}
//
//void pdfReader::ReadTrailer()
//{
//	pdfAtom atm = ReadObj();
//	if ( atm.GetType() == Dictionary )
//	{
//		pdfDictionary dictTrailer(atm);
//		if( doc.m_Trailer.IsNull() )
//			doc.m_Trailer = dictTrailer;
//
//		if( dictTrailer.Has( "Prev" ) )
//		{
//			SetCursor( dictTrailer["Prev"].Int() );
//			ReadXRef();
//		}
//
//#ifdef _DEBUG
//		//DumpDict(Keys.trailer, m_dictTrailer);
//#endif
//	}
//}
//
//} ; //namespace Utils
} ; //namespace Pdf
