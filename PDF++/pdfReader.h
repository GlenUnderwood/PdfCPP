#pragma once
#include "ExportDef.h"
#include "Atom.h"
#include "DocData.h"
#include <iostream>
#include <fstream>

namespace Pdf{

class pdfReader
{
	pdfReader();
public:
	pdfReader( const CDocData::Ptr& doc, int iNewCursor = 0 );
	pdfReader( const CDocData::Ptr& doc, std::istream* pStm, int iNewCursor = 0 );
	virtual ~pdfReader(void);

	void Parse();

	void SetCursor( int iCursor );
	inline int GetCursor() const  throw() { return _iCursor; };
	inline char Current(char c) const  throw() { return _Current; };
	inline char Next(char c) const  throw() { return _Next; };
	inline char Previous(char c) const  throw() { return _Previous; };

	bool IsWhiteSpace(char c) const throw();
	bool IsDelimiter(char c) const throw();
	char GotoNextChar();
	void SkipWhiteSpace();
	void ReadEscapeChar();
	void SkipToEOL();
	void SkipComment();
	byte FromHexDigit(char cDigit);
	bool IsAtHexByte() const throw();

	virtual bool IsValidKeyword(const char* sVal);

	virtual pdfAtom ReadNum();
	virtual pdfAtom ReadName();
	virtual pdfAtom ReadString();
	virtual pdfAtom ReadHexString();
	virtual pdfAtom ReadDict();
	virtual pdfAtom ReadArray();
	virtual pdfAtom ReadObj();

	void Reset( std::istream* pstm, int iNewCursor = 0 );
protected:

	CDocData::WeakPtr m_doc;

	std::istream* m_stm;

	char _Current, _Next, _Previous;
	int64 _iCursor;

	StringEncoding m_strDefStrEncoding;
	std::stringbuf m_bufStrings;
//	std::ostringstream m_stmStrings;
	bool m_bAssumeDocIsEncrypted;
};

//}; //namespace Utils
}; //namespace Pdf






