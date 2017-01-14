#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfColorSpace : public pdfDictionary//pdfObject
{
protected:
	pdfColorSpace(pdfDocument& doc);
public:
	pdfColorSpace(void);
	pdfColorSpace(const pdfAtom& src);
	virtual ~pdfColorSpace(void);

	pdfColorSpace& operator=(const pdfAtom& src);
	inline pdfColorSpace& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

};

};// namespace Exception
