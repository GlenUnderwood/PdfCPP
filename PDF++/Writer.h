#pragma once
#include "ExportDef.h"
#include "Atom.h"
#include "DocData.h"
#include <iostream>
#include <fstream>

namespace Pdf{

class pdfDocument;

// Available for public use if needed.
class PDF_API pdfWriter
{
public:
	pdfWriter();
	//pdfWriter(const pdfDocument& doc);
	virtual ~pdfWriter(void);

	int GetMaxCharsPerLine() ;
	void SetMaxCharsPerLine( const int i );

	OutputStyle GetOutputStyle() ;
	void SetOutputStyle( const OutputStyle i );

	int GetIndent();
	void SetIndent( const int value );

	void Write( const pdfDocument& doc, std::ostream& stm );

protected:
	void WriteAtom( const pdfDocument& doc, std::ostream& stm, pdfAtom& atm, int iIndent );
	void WriteNameChar( std::ostream& stm, char c );
	void WriteChar( std::ostream& stm, byte c );
	void WriteString( std::ostream& stm, pdfString& d );
	void WriteName( std::ostream& stm, pdfName& d );
	void WriteHexString( std::ostream& stm, pdfString& d );
	void WriteDict( const pdfDocument& doc, std::ostream& stm, pdfDictionary& d, int iIndent );
	void WriteStream( const pdfDocument& doc, std::ostream& stm, pdfBlob& d, int iIndent );
	void WriteArray( const pdfDocument& doc, std::ostream& stm, pdfArray& d, int iIndent );
	void WriteReal( std::ostream& stm, pdfReal& real );
	void WriteIndent( std::ostream& stm, int iIndent );

	void Decrypt( const pdfDocument& doc );
	void DecryptAtom( pdfAtom& atm );

	bool IsDelimiter(char c) const throw();

	//CDocData::Ptr m_doc;
	Security::pdfSecurityHandler* m_secEncryptor;
	Security::pdfSecurityHandler* m_secDecryptor;

	bool m_bSuspendEncryption;

	int m_iIndent, m_iMaxCharsPerLine;
	OutputStyle m_OutputStyle ;

	char m_szBuff[25];

	std::map<int, string> m_mapIndents;
};

}; //namespace Pdf






