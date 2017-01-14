#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfFormXObject : public pdfDictionary//pdfObject
{
protected:
	pdfFormXObject(pdfDocument& doc);
public:
	pdfFormXObject(void);
	pdfFormXObject(const pdfAtom& src);
	virtual ~pdfFormXObject(void);

	pdfFormXObject& operator=(const pdfAtom& src);
	inline pdfFormXObject& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

};

};// namespace Exception
