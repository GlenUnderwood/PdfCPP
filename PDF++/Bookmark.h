#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfBookmark : public pdfDictionary//pdfObject
{
protected:
	pdfBookmark(pdfDocument& doc);
public:
	pdfBookmark(void);
	pdfBookmark(const pdfAtom& src);
	virtual ~pdfBookmark(void);

	pdfBookmark& operator=(const pdfAtom& src);
	inline pdfBookmark& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}
};

};// namespace Exception
