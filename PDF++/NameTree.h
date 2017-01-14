#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfNameTree : public pdfDictionary//pdfObject
{
protected:
	pdfNameTree(pdfDocument& doc);
public:
	pdfNameTree(void);
	pdfNameTree(const pdfAtom& src);
	virtual ~pdfNameTree(void);

	pdfNameTree& operator=(const pdfAtom& src);
	inline pdfNameTree& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}
};

};// namespace Exception
