#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfImageObj : public pdfDictionary//pdfObject
{
protected:
	pdfImageObj(pdfDocument& doc);
public:
	pdfImageObj(void);
	pdfImageObj(const pdfAtom& src);
	virtual ~pdfImageObj(void);

	pdfImageObj& operator=(const pdfAtom& src);
	inline pdfImageObj& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

};

};// namespace Exception
