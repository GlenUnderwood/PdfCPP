#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfAction : public pdfDictionary//pdfObject
{
protected:
	pdfAction(pdfDocument& doc);
public:
	pdfAction(void);
	pdfAction(const pdfAtom& src);
	virtual ~pdfAction(void);

	pdfAction& operator=(const pdfAtom& src);
	inline pdfAction& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

};

};// namespace Exception
