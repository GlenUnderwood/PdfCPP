#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfBookmarks : public pdfDictionary//pdfObject
{
protected:
	pdfBookmarks(pdfDocument& doc);
public:
	pdfBookmarks(void);
	pdfBookmarks(const pdfAtom& src);
	virtual ~pdfBookmarks(void);

	pdfBookmarks& operator=(const pdfAtom& src);
	inline pdfBookmarks& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}
};

};// namespace Exception
