#pragma once
#include "ExportDef.h"
#include "Dictionary.h"
#include "Resources.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfContents : public pdfObject
{
//protected:
	//pdfContents(pdfDocument& doc);
public:
	pdfContents(void);
	pdfContents(const pdfAtom& src);
	virtual ~pdfContents(void);

	pdfContents& operator=(const pdfAtom& src);
	inline pdfContents& operator=(const std::nullptr_t i) { m_atm.SetValue( i ); return *this;}

protected:
	void CheckIsValid() const; 

};

};// namespace Exception
